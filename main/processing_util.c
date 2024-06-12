#include "processing_util.h"

static int		audio_input_paused = 0;
static uint8_t	audio_input_ring_buffer_storage[2 * 8000];
static btstack_ring_buffer_t audio_input_ring_buffer;

static int		audio_output_paused = 0;
static uint8_t	audio_output_ring_buffer_storage[2 * MSBC_PA_PREBUFFER_BYTES];
static btstack_ring_buffer_t audio_output_ring_buffer;

static int	dump_data = 1;
static int	count_sent = 0;
static int	count_received = 0;
static int	negotiated_codec = -1;

static btstack_sbc_decoder_state_t decoder_state;

int	num_audio_frames;
int	num_samples_to_write;
unsigned int phase;

static void playback_callback(int16_t *buffer, uint16_t num_samples)
{
    uint32_t prebuffer_bytes;
	prebuffer_bytes = MSBC_PA_PREBUFFER_BYTES;

    if (audio_output_paused)
    {
        if (btstack_ring_buffer_bytes_available(&audio_output_ring_buffer) < prebuffer_bytes)
        {
            memset(buffer, 0, num_samples * BYTES_PER_FRAME);
            return;
        }
        audio_output_paused = 0;
    }

    uint32_t bytes_read = 0;
    btstack_ring_buffer_read(&audio_output_ring_buffer, (uint8_t *)buffer, num_samples * BYTES_PER_FRAME, &bytes_read);
    num_samples -= bytes_read / BYTES_PER_FRAME;
    buffer += bytes_read / BYTES_PER_FRAME;

    if (num_samples)
    {
        memset(buffer, 0, num_samples * BYTES_PER_FRAME);
        audio_output_paused = 1;
    }
}

static void recording_callback(const int16_t *buffer, uint16_t num_samples)
{
    btstack_ring_buffer_write(&audio_input_ring_buffer, (uint8_t *)buffer, num_samples * 2);
}

static int audio_initialize(int sample_rate)
{
    memset(audio_output_ring_buffer_storage, 0, sizeof(audio_output_ring_buffer_storage));
    btstack_ring_buffer_init(&audio_output_ring_buffer, audio_output_ring_buffer_storage, sizeof(audio_output_ring_buffer_storage));

    const btstack_audio_sink_t *audio_sink = btstack_audio_sink_get_instance();
    if (audio_sink)
    {
        audio_sink->init(1, sample_rate, &playback_callback);
        audio_sink->start_stream();
        audio_output_paused = 1;
    }

    memset(audio_input_ring_buffer_storage, 0, sizeof(audio_input_ring_buffer_storage));
    btstack_ring_buffer_init(&audio_input_ring_buffer, audio_input_ring_buffer_storage, sizeof(audio_input_ring_buffer_storage));

    const btstack_audio_source_t *audio_source = btstack_audio_source_get_instance();
    if (audio_source)
    {
        audio_source->init(1, sample_rate, &recording_callback);
        audio_source->start_stream();
        audio_input_paused = 1;
    }
    return 1;
}

static void audio_terminate(void)
{
    const btstack_audio_sink_t *audio_sink = btstack_audio_sink_get_instance();
    if (audio_sink)
    {
        audio_sink->close();
    }

    const btstack_audio_source_t *audio_source = btstack_audio_source_get_instance();
    if (audio_source)
    {
        audio_source->close();
    }
}

static void handle_pcm_data(int16_t *data, int num_samples, int num_channels, int sample_rate, void *context)
{
    UNUSED(context);
    UNUSED(sample_rate);
    UNUSED(data);
    UNUSED(num_samples);
    UNUSED(num_channels);

    btstack_ring_buffer_write(&audio_output_ring_buffer, (uint8_t *)data, num_samples * num_channels * 2);
}

static void processing_init_mSBC(void)
{
    printf("Processing: Init mSBC\n");

    btstack_sbc_decoder_init(&decoder_state, SBC_MODE_mSBC, &handle_pcm_data, NULL);
    hfp_msbc_init();

    audio_initialize(MSBC_SAMPLE_RATE);
}

static void processing_receive_mSBC(uint8_t *packet, uint16_t size)
{
    btstack_sbc_decoder_process_data(&decoder_state, (packet[1] >> 4) & 3, packet + 3, size - 3);
}

void processing_close(void)
{
    printf("SCO demo close\n");

    printf("SCO demo statistics: ");
    printf("Used mSBC with PLC, number of processed frames: \n - %d good frames, \n - %d zero frames, \n - %d bad frames.\n", decoder_state.good_frames_nr, decoder_state.zero_frames_nr, decoder_state.bad_frames_nr);
    audio_terminate();
}

void processing_set_codec(uint8_t codec)
{
    if (negotiated_codec == codec)
        return;
    negotiated_codec = codec;
    processing_init_mSBC();
}

void processing_init(void)
{
    printf("Processing: Sending and receiving audio via btstack_audio.\n");
    hci_set_sco_voice_setting(0x60);
}

void sco_report(void)
{
    printf("SCO: sent %u, received %u\n", count_sent, count_received);
}

void processing_send(hci_con_handle_t sco_handle)
{
    if (sco_handle == HCI_CON_HANDLE_INVALID)
        return;

    int sco_packet_length = hci_get_sco_packet_length();
    int sco_payload_length = sco_packet_length - 3;

    hci_reserve_packet_buffer();
    uint8_t *sco_packet = hci_get_outgoing_packet_buffer();

    if (btstack_audio_source_get_instance())
    {
		if (audio_input_paused)
		{
			if (btstack_ring_buffer_bytes_available(&audio_input_ring_buffer) >= MSBC_PA_PREBUFFER_BYTES)
			{
				audio_input_paused = 0;
			}
		}

		if (!audio_input_paused)
		{
			int num_samples = hfp_msbc_num_audio_samples_per_frame();
			if (num_samples > MAX_NUM_MSBC_SAMPLES)
				return;
			if (hfp_msbc_can_encode_audio_frame_now() && btstack_ring_buffer_bytes_available(&audio_input_ring_buffer) >= (unsigned int)(num_samples * BYTES_PER_FRAME))
			{
				int16_t sample_buffer[MAX_NUM_MSBC_SAMPLES];
				uint32_t bytes_read;
				btstack_ring_buffer_read(&audio_input_ring_buffer, (uint8_t *)sample_buffer, num_samples * BYTES_PER_FRAME, &bytes_read);
				hfp_msbc_encode_audio_frame(sample_buffer);
				num_audio_frames++;
			}
			if (hfp_msbc_num_bytes_in_stream() < sco_payload_length)
			{
				log_error("mSBC stream should not be empty.");
			}
		}

		if (audio_input_paused || hfp_msbc_num_bytes_in_stream() < sco_payload_length)
		{
			memset(sco_packet + 3, 0, sco_payload_length);
			audio_input_paused = 1;
		}
		else
		{
			hfp_msbc_read_from_stream(sco_packet + 3, sco_payload_length);
		}
    }
    else
    {
        memset(sco_packet + 3, 0, sco_payload_length);
    }
    little_endian_store_16(sco_packet, 0, sco_handle);
    sco_packet[2] = sco_payload_length;
    hci_send_sco_packet_buffer(sco_packet_length);
    hci_request_sco_can_send_now_event();

    count_sent++;
    if ((count_sent % SCO_REPORT_PERIOD) == 0)
        sco_report();
}

void processing_receive(uint8_t *packet, uint16_t size)
{

    dump_data = 1;

    count_received++;
    static uint32_t packets = 0;
    static uint32_t crc_errors = 0;
    static uint32_t data_received = 0;
    static uint32_t byte_errors = 0;

    data_received += size - 3;
    packets++;
    if (data_received > 100000)
    {
        printf("Summary: data %07u, packets %04u, packet with crc errors %0u, byte errors %04u\n", (unsigned int)data_received, (unsigned int)packets, (unsigned int)crc_errors, (unsigned int)byte_errors);
        crc_errors = 0;
        byte_errors = 0;
        data_received = 0;
        packets = 0;
    }

    switch (negotiated_codec)
    {
		case HFP_CODEC_MSBC:
			processing_receive_mSBC(packet, size);
			break;
		default:
			break;
    }
    dump_data = 0;
}

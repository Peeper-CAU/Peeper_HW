#include "hfp_util.h"

uint8_t			hfp_service_buffer[150];
const char		*hfp_device_name = "PEEPER";
const uint8_t	rfcomm_channel_nr = 1;
static uint8_t	codecs[] = {HFP_CODEC_CVSD, HFP_CODEC_MSBC};
static uint16_t	indicators[1] = {0x01};
static btstack_packet_callback_registration_t	hci_event_callback_registration;
static hci_con_handle_t sco_handle = HCI_CON_HANDLE_INVALID;

static void	hfp_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    if (packet_type == HCI_SCO_DATA_PACKET)
	{
		// PCM Data Packet
	}
	else
	{
        switch (hci_event_packet_get_type(packet))
		{
            case HCI_EVENT_SCO_CAN_SEND_NOW:
				printf("Sendable from Now");
				break;
            case HCI_EVENT_COMMAND_COMPLETE:
                printf("Command complete\n");
                break;
			case HCI_EVENT_HFP_META:
				switch(hci_event_hfp_meta_get_subevent_code(packet))
				{
					case HFP_SUBEVENT_RING:
						printf("** Ring **\n");
						break;
					case HFP_SUBEVENT_CALLING_LINE_IDENTIFICATION_NOTIFICATION:
						printf("Caller ID, number %s\n", hfp_subevent_calling_line_identification_notification_get_number(packet));
						break;
				}
            default:
                break;
        }
    }
}

void	init_hfp(void)
{
	printf("## Initializing HFP\n");
    gap_discoverable_control(1);
    gap_set_class_of_device(0x200408);
    gap_set_local_name(hfp_device_name);

    l2cap_init();

    uint16_t hf_supported_features =
        (1 << HFP_HFSF_ESCO_S4) |
        (1 << HFP_HFSF_CLI_PRESENTATION_CAPABILITY) |
        (1 << HFP_HFSF_HF_INDICATORS) |
        (1 << HFP_HFSF_CODEC_NEGOTIATION) |
        (1 << HFP_HFSF_ENHANCED_CALL_STATUS) |
        (1 << HFP_HFSF_REMOTE_VOLUME_CONTROL);
    int wide_band_speech = 1;

    rfcomm_init();
    hfp_hf_init(rfcomm_channel_nr);
    hfp_hf_init_supported_features(hf_supported_features);
    hfp_hf_init_hf_indicators(sizeof(indicators) / sizeof(uint16_t), indicators);
    hfp_hf_init_codecs(sizeof(codecs), codecs);

    sdp_init();
    memset(hfp_service_buffer, 0, sizeof(hfp_service_buffer));
    hfp_hf_create_sdp_record(hfp_service_buffer, 0x10001, rfcomm_channel_nr, hfp_device_name, hf_supported_features, wide_band_speech);
    sdp_register_service(hfp_service_buffer);

    hci_event_callback_registration.callback = &hfp_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    hci_register_sco_packet_handler(&hfp_packet_handler);
    hci_register_sco_packet_handler(&hfp_packet_handler);

    hfp_hf_register_packet_handler(hfp_packet_handler);

    hci_power_control(HCI_POWER_ON);
	printf("## Initialized HFP\n");
}

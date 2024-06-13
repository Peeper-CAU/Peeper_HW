#include "hfp_util.h"

uint8_t			hfp_service_buffer[150];
const char		*hfp_device_addr = "E4:65:B8:76:CE:2A";
const char		*hfp_device_name = "PEEPER";
const uint8_t	rfcomm_channel_nr = 1;
static uint8_t	codecs[] = {HFP_CODEC_CVSD, HFP_CODEC_MSBC};
static uint8_t	negotiated_codec = HFP_CODEC_CVSD;
static uint16_t	indicators[1] = {0x01};
static btstack_packet_callback_registration_t	hci_event_callback_registration;
static hci_con_handle_t	acl_handle = HCI_CON_HANDLE_INVALID;
static hci_con_handle_t	sco_handle = HCI_CON_HANDLE_INVALID;
static bd_addr_t device_addr;

static void	hfp_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *event, uint16_t event_size)
{
    UNUSED(channel);
    uint8_t status;

    switch (packet_type)
    {

    case HCI_SCO_DATA_PACKET:
        if (READ_SCO_CONNECTION_HANDLE(event) != sco_handle)
            break;
        processing_receive(event, event_size);
        break;

    case HCI_EVENT_PACKET:
        switch (hci_event_packet_get_type(event))
        {
        case HCI_EVENT_SCO_CAN_SEND_NOW:
            // processing_send(sco_handle);
            break;

        case HCI_EVENT_HFP_META:
            switch (hci_event_hfp_meta_get_subevent_code(event))
            {
            case HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_ESTABLISHED:
                acl_handle = hfp_subevent_service_level_connection_established_get_acl_handle(event);
                hfp_subevent_service_level_connection_established_get_bd_addr(event, device_addr);
                printf("Service level connection established %s.\n\n", bd_addr_to_str(device_addr));
                break;
            case HFP_SUBEVENT_SERVICE_LEVEL_CONNECTION_RELEASED:
                acl_handle = HCI_CON_HANDLE_INVALID;
                printf("Service level connection released.\n\n");
                break;
            case HFP_SUBEVENT_AUDIO_CONNECTION_ESTABLISHED:
                status = hfp_subevent_audio_connection_established_get_status(event);
                if (status != ERROR_CODE_SUCCESS)
                {
                    printf("Audio connection establishment failed with status 0x%02x\n", status);
                }
                else
                {
                    sco_handle = hfp_subevent_audio_connection_established_get_sco_handle(event);
                    printf("Audio connection established with SCO handle 0x%04x.\n", sco_handle);
                    negotiated_codec = hfp_subevent_audio_connection_established_get_negotiated_codec(event);
                    switch (negotiated_codec)
                    {
                    case 0x01:
                        printf("Using CVSD codec.\n");
                        break;
                    case 0x02:
                        printf("Using mSBC codec.\n");
                        break;
                    default:
                        printf("Using unknown codec 0x%02x.\n", negotiated_codec);
                        break;
                    }
                    processing_set_codec(negotiated_codec);
                    hci_request_sco_can_send_now_event();
                }
                break;
            case HFP_SUBEVENT_AUDIO_CONNECTION_RELEASED:
                sco_handle = HCI_CON_HANDLE_INVALID;
                printf("Audio connection released\n");
                processing_close();
                break;
            case HFP_SUBEVENT_AG_INDICATOR_STATUS_CHANGED:
                printf("AG_INDICATOR_STATUS_CHANGED, AG indicator (index: %d) to: %d of range [%d, %d], name '%s'\n",
                       hfp_subevent_ag_indicator_status_changed_get_indicator_index(event),
                       hfp_subevent_ag_indicator_status_changed_get_indicator_status(event),
                       hfp_subevent_ag_indicator_status_changed_get_indicator_min_range(event),
                       hfp_subevent_ag_indicator_status_changed_get_indicator_max_range(event),
                       (const char *)hfp_subevent_ag_indicator_status_changed_get_indicator_name(event));
                break;
            case HFP_SUBEVENT_NETWORK_OPERATOR_CHANGED:
                printf("NETWORK_OPERATOR_CHANGED, operator mode: %d, format: %d, name: %s\n",
                       hfp_subevent_network_operator_changed_get_network_operator_mode(event),
                       hfp_subevent_network_operator_changed_get_network_operator_format(event),
                       (char *)hfp_subevent_network_operator_changed_get_network_operator_name(event));
                break;
            case HFP_SUBEVENT_EXTENDED_AUDIO_GATEWAY_ERROR:
                printf("EXTENDED_AUDIO_GATEWAY_ERROR_REPORT, status : 0x%02x\n",
                       hfp_subevent_extended_audio_gateway_error_get_error(event));
                break;
            case HFP_SUBEVENT_RING:
                printf("** Ring **\n");
                break;
            case HFP_SUBEVENT_NUMBER_FOR_VOICE_TAG:
                printf("Phone number for voice tag: %s\n",
                       (const char *)hfp_subevent_number_for_voice_tag_get_number(event));
                break;
            case HFP_SUBEVENT_SPEAKER_VOLUME:
                printf("Speaker volume: gain %u\n",
                       hfp_subevent_speaker_volume_get_gain(event));
                break;
            case HFP_SUBEVENT_MICROPHONE_VOLUME:
                printf("Microphone volume: gain %u\n",
                       hfp_subevent_microphone_volume_get_gain(event));
                break;
            case HFP_SUBEVENT_CALLING_LINE_IDENTIFICATION_NOTIFICATION:
                printf("Caller ID, number %s\n", hfp_subevent_calling_line_identification_notification_get_number(event));
                break;
            case HFP_SUBEVENT_ENHANCED_CALL_STATUS:
                printf("Enhanced call status:\n");
                printf("  - call index: %d \n", hfp_subevent_enhanced_call_status_get_clcc_idx(event));
                printf("  - direction : %s \n", hfp_enhanced_call_dir2str(hfp_subevent_enhanced_call_status_get_clcc_dir(event)));
                printf("  - status    : %s \n", hfp_enhanced_call_status2str(hfp_subevent_enhanced_call_status_get_clcc_status(event)));
                printf("  - mode      : %s \n", hfp_enhanced_call_mode2str(hfp_subevent_enhanced_call_status_get_clcc_mode(event)));
                printf("  - multipart : %s \n", hfp_enhanced_call_mpty2str(hfp_subevent_enhanced_call_status_get_clcc_mpty(event)));
                printf("  - type      : %d \n", hfp_subevent_enhanced_call_status_get_bnip_type(event));
                printf("  - number    : %s \n", hfp_subevent_enhanced_call_status_get_bnip_number(event));
                break;
            default:
                break;
            }
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void	init_hfp(void)
{
	printf("## Initializing HFP\n");
	processing_init();

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
	printf("SDP service record size: %lu\n", de_get_len(hfp_service_buffer));

    hci_event_callback_registration.callback = &hfp_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);
    hci_register_sco_packet_handler(&hfp_packet_handler);

    hfp_hf_register_packet_handler(hfp_packet_handler);

    hci_power_control(HCI_POWER_ON);
	sscanf_bd_addr(hfp_device_addr, device_addr);
	printf("## Initialized HFP\n");
}

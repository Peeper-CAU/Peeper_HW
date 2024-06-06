#include "ble_util.h"

static const uint8_t adv_data[] = {
    0x02, 0x01, 0x06,
    0x03, 0x03, 0xAB, 0xCD,
    0x0B, 0x09, 'P', 'E', 'E', 'P', 'E', 'R', '_', 'B', 'L', 'E'
};

static void	ble_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    if (packet_type == HCI_EVENT_PACKET)
	{
        switch (hci_event_packet_get_type(packet))
		{
            case GAP_EVENT_ADVERTISING_REPORT:
                printf("Advertising report received\n");
                break;
            case HCI_EVENT_COMMAND_COMPLETE:
                printf("Command complete\n");
                break;
            case HCI_EVENT_LE_META:
                switch (hci_event_le_meta_get_subevent_code(packet))
				{
                    case HCI_SUBEVENT_LE_CONNECTION_COMPLETE:
                        printf("LE Connection complete\n");
                        break;
                }
                break;
            default:
                break;
        }
    }
}

void	init_ble(void)
{
    l2cap_init();
    sm_init();
    gap_advertisements_set_params(0x0030, 0x0030, 0x00, 0x00, NULL, 0x07, 0x00);
    gap_advertisements_set_data(sizeof(adv_data), (uint8_t*)adv_data);
    gap_advertisements_enable(1);
	btstack_run_loop_execute();
}

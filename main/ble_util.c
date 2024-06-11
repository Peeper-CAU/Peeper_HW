#include "ble_util.h"

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
                // printf("Command complete\n");
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
	printf("## Initializing BLE\n");
	l2cap_init();
	sm_init();

    gap_advertisements_enable(1);

	static btstack_packet_callback_registration_t hci_event_callback_registration;
    hci_event_callback_registration.callback = &ble_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    hci_power_control(HCI_POWER_ON);
	printf("## Initialized BLE\n");
}

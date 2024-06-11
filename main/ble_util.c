#include "ble_util.h"

static void	ble_packet_handler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size)
{
    if (packet_type == HCI_EVENT_PACKET)
	{
        switch (hci_event_packet_get_type(packet))
        {
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

	uint16_t adv_interval_min = 0x0020;
    uint16_t adv_interval_max = 0x0030;
    uint8_t adv_type = 0x00;
    uint8_t own_address_type = 0x00;
    uint8_t direct_address_type = 0x00;
    uint8_t direct_address[6] = {0};
    uint8_t adv_channel_map = 0x07;
    uint8_t adv_filter_policy = 0x00;
    gap_advertisements_set_params(adv_interval_min, adv_interval_max, adv_type,
                                  direct_address_type, direct_address,
                                  adv_channel_map, adv_filter_policy);

	uint8_t adv_data[] = { 0x02, 0x01, 0x06 };
    gap_advertisements_set_data(sizeof(adv_data), adv_data);
    gap_advertisements_enable(1);

	static btstack_packet_callback_registration_t hci_event_callback_registration;
    hci_event_callback_registration.callback = &ble_packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    hci_power_control(HCI_POWER_ON);
	printf("## Initialized BLE\n");
}

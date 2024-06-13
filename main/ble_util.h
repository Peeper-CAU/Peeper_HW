#ifndef BLE_UTIL_H
# define BLE_UTIL_H

# include "btstack.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "gap.h"
#include "hci.h"
#include "l2cap.h"

void	ble_send_data(uint8_t *data, uint16_t length);
void	init_ble(void);

#endif

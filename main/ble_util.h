#ifndef BLE_UTIL_H
# define BLE_UTIL_H

# include "btstack.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "hci.h"
#include "hci_dump.h"
#include "l2cap.h"
#include "rfcomm.h"
#include "sdp_server.h"
#include "hfp_hf.h"

void	init_ble(void);

#endif

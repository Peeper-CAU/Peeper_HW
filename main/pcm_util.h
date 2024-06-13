#ifndef BLE_PCM_UTIL_H
# define BLE_PCM_UTIL_H

# include <stdint.h>
# include <stdio.h>
# include <string.h>

# include "ble_util.h"

void	log_pcm_data(const int16_t *pcm_data, int num_samples);
void	send_pcm_data(const int16_t *pcm_data, int num_samples);

#endif

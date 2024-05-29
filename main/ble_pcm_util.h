#ifndef BLE_PCM_UTIL_H
# define BLE_PCM_UTIL_H

# include <stdint.h>
# include <stdio.h>

# define PCM_BUFFER_SIZE 240

extern int16_t	pcm_data[PCM_BUFFER_SIZE];
extern int		pcm_data_size;

void	log_pcm_data(const int16_t *pcm_data, int num_samples);

#endif

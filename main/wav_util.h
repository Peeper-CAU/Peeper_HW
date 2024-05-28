#ifndef WAV_UTIL_H
# define WAV_UTIL_H

# include "stdint.h"
# define PCM_BUFFER_SIZE 240

int16_t	pcm_data[PCM_BUFFER_SIZE];
int		pcm_data_size = 0;

void	log_pcm_data(const int16_t *pcm_data, int num_samples);

#endif

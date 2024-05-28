#ifndef WAV_UTIL_H
# define WAV_UTIL_H

# include <stdint.h>
# include <stdio.h>
# define PCM_BUFFER_SIZE 240

FILE	*output_wav_file;
int16_t	pcm_data[PCM_BUFFER_SIZE];
int		pcm_data_size = 0;
int		total_wav_samples = 0;

void	log_pcm_data(const int16_t *pcm_data, int num_samples);

void	close_output_wav_file();
void	init_output_wav_file(const char *filename);

#endif

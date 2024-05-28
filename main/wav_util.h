#ifndef WAV_UTIL_H
# define WAV_UTIL_H

# include <stdint.h>
# include <stdio.h>
# define PCM_BUFFER_SIZE 240

int16_t	pcm_data[PCM_BUFFER_SIZE];
int		pcm_data_size = 0;

void	log_pcm_data(const int16_t *pcm_data, int num_samples);

void	close_wav_file();
void	init_wav_file(const char *filename);
void	write_wav_header(FILE *file, int sample_rate, int num_channels, int bits_per_sample, int num_samples);
#endif

#ifndef WAV_UTIL_H
# define WAV_UTIL_H

# include <stdint.h>
# include <stdio.h>
# define PCM_BUFFER_SIZE 240

extern FILE	*output_wav_file;
extern int16_t	pcm_data[PCM_BUFFER_SIZE];
extern int		pcm_data_size;
extern int		total_wav_samples;

void	log_pcm_data(const int16_t *pcm_data, int num_samples);

void	close_output_wav_file();
void	init_output_wav_file(const char *filename);

#endif

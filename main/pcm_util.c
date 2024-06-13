#include "pcm_util.h"

void	log_pcm_data(const int16_t *pcm_data, int num_samples)
{
	printf("(Size %d) : ", num_samples);
	int log_size = num_samples < 10 ? num_samples : 10;
    for(int i = 0; i < log_size; i++) {
        printf("%d ", pcm_data[i]);
    }
    printf("\n");
}

void	send_pcm_data(const int16_t *pcm_data, int num_samples)
{
    uint8_t	output_buffer[num_samples * 2];

    for(int i = 0; i < num_samples; i++)
	{
        output_buffer[i * 2] = (uint8_t)(pcm_data[i] & 0xFF);
        output_buffer[i * 2 + 1] = (uint8_t)((pcm_data[i] >> 8) & 0xFF);
    }

	for(int i = 0; i < num_samples * 2; i += 10){
		ble_send_data(output_buffer + i, 10);
	}
}

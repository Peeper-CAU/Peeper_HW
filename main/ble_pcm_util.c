#include "ble_pcm_util.h"

int16_t	pcm_data[PCM_BUFFER_SIZE];
int		pcm_data_size = 0;

void log_pcm_data(const int16_t *pcm_data, int num_samples) {
	printf("%d (Size %d)", pcm_data[0], pcm_data_size);
    // for (int i = 0; i < num_samples; i++) {
    //     printf("%d ", pcm_data[i]);
    //     if (i % 20 == 19) {
    //         printf("\n");
    //     }
    // }
    printf("\n");
}

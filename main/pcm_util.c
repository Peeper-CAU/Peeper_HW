#include "pcm_util.h"

int16_t	pcm_data[PCM_BUFFER_SIZE];
int		pcm_data_size = 0;

void log_pcm_data(const int16_t *pcm_data, int num_samples) {
	printf("(Size %d) : ", num_samples);
	int log_size = num_samples < 10 ? num_samples : 10;
    for(int i = 0; i < log_size; i++) {
        printf("%d ", pcm_data[i]);
    }
    printf("\n");
}

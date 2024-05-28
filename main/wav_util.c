#include "wav_util.h"
#define PCM_BUFFER_SIZE 240

int16_t pcm_data[PCM_BUFFER_SIZE];
int pcm_data_size = 0;

void log_pcm_data(const int16_t *pcm_data, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        printf("%d ", pcm_data[i]);
        if (i % 20 == 19) {
            printf("\n");
        }
    }
    printf("\n");
}

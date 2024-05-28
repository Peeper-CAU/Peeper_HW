#include "wav_util.h"

void write_wav_header(FILE *file, int sample_rate, int num_channels, int bits_per_sample, int num_samples);

FILE	*output_wav_file;
int16_t	pcm_data[PCM_BUFFER_SIZE];
int		pcm_data_size = 0;
int		total_wav_samples = 0;

void log_pcm_data(const int16_t *pcm_data, int num_samples) {
    for (int i = 0; i < num_samples; i++) {
        printf("%d ", pcm_data[i]);
        if (i % 20 == 19) {
            printf("\n");
        }
    }
    printf("\n");
}

void close_output_wav_file() {
    if (output_wav_file) {
        fseek(output_wav_file, 0, SEEK_SET);
        write_wav_header(output_wav_file, 16000, 1, 16, total_wav_samples);
        fclose(output_wav_file);
        output_wav_file = NULL;
    }
}

void init_output_wav_file(const char *filename) {
    output_wav_file = fopen(filename, "wb");
    if (!output_wav_file) {
        printf("Failed to open WAV file\n");
        return;
    }

    write_wav_header(output_wav_file, 16000, 1, 16, 0);
}

void write_wav_header(FILE *file, int sample_rate, int num_channels, int bits_per_sample, int num_samples) {
    int byte_rate = sample_rate * num_channels * bits_per_sample / 8;
    int block_align = num_channels * bits_per_sample / 8;
    int subchunk2_size = num_samples * num_channels * bits_per_sample / 8;
    int chunk_size = 36 + subchunk2_size;

    fwrite("RIFF", 1, 4, file);
    fwrite(&chunk_size, 4, 1, file);
    fwrite("WAVE", 1, 4, file);
    fwrite("fmt ", 1, 4, file);

    int subchunk1_size = 16;
    int audio_format = 1;
    fwrite(&subchunk1_size, 4, 1, file);
    fwrite(&audio_format, 2, 1, file);
    fwrite(&num_channels, 2, 1, file);
    fwrite(&sample_rate, 4, 1, file);
    fwrite(&byte_rate, 4, 1, file);
    fwrite(&block_align, 2, 1, file);
    fwrite(&bits_per_sample, 2, 1, file);

    fwrite("data", 1, 4, file);
    fwrite(&subchunk2_size, 4, 1, file);
}

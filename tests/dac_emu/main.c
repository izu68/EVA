#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio/miniaudio.h"
#include <stdint.h>
#include <string.h>

#define SAMPLE_RATE 44100
#define BUFFER_SIZE 256

// Circular buffer for audio samples
uint8_t audioBuffer[BUFFER_SIZE * 2]; // Stereo: Two channels
volatile int bufferWriteIndex = 0;
volatile int bufferReadIndex = 0;

// DAC emulation function
void dac(uint8_t ch1_sample, uint8_t ch2_sample) {
    // Write stereo samples to the circular buffer
    audioBuffer[bufferWriteIndex] = ch1_sample;      // Channel 1
    audioBuffer[bufferWriteIndex + 1] = ch2_sample;  // Channel 2

    // Increment write index (wrap around for circular buffer)
    bufferWriteIndex = (bufferWriteIndex + 2) % (BUFFER_SIZE * 2);
}

// Miniaudio playback callback
void dataCallback(ma_device *pDevice, void *pOutput, const void *pInput, ma_uint32 frameCount) {
    uint8_t *output = (uint8_t *)pOutput;

    for (ma_uint32 i = 0; i < frameCount; i++) {
        // Read samples from the circular buffer
        output[2 * i] = audioBuffer[bufferReadIndex];       // Left channel
        output[2 * i + 1] = audioBuffer[bufferReadIndex + 1]; // Right channel

        // Increment read index (wrap around for circular buffer)
        bufferReadIndex = (bufferReadIndex + 2) % (BUFFER_SIZE * 2);
    }

    (void)pInput; // Unused
}


// Function to play a raw PCM file
void playPcmFile(const char *filePath) {
    FILE *file = fopen(filePath, "rb");
    if (!file) {
        printf("Failed to open PCM file: %s\n", filePath);
        return;
    }

    // Buffer for reading PCM samples (stereo: 2 channels, 256 frames)
    uint8_t buffer[256 * 2];

    // Read and feed PCM data to the DAC
    while (1) {
        size_t bytesRead = fread(buffer, 1, sizeof(buffer), file);
        if (bytesRead == 0) break; // End of file

        // Feed samples to the DAC
        for (size_t i = 0; i < bytesRead; i += 2) {
            uint8_t leftSample = buffer[i];     // Left channel
            uint8_t rightSample = buffer[i + 1]; // Right channel
            dac(leftSample, rightSample);
        }
    }

    fclose(file);
}

int main() {
    // Audio playback setup (same as before)
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.format = ma_format_u8;       // Unsigned 8-bit PCM
    config.playback.channels = 2;               // Stereo
    config.sampleRate = SAMPLE_RATE;
    config.dataCallback = dataCallback;

    ma_device device;
    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
        printf("Failed to initialize playback device.\n");
        return -1;
    }

    if (ma_device_start(&device) != MA_SUCCESS) {
        printf("Failed to start playback device.\n");
        ma_device_uninit(&device);
        return -1;
    }

    // Play a raw PCM file
    playPcmFile("audio.pcm");

    // Wait a moment before shutting down (to let the audio finish playing)
    ma_sleep(1000);

    // Clean up
    ma_device_uninit(&device);
    return 0;
}

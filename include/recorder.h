#ifndef RECORDER_H
#define RECORDER_H

// #include <stdint.h>
#include <driver/i2s.h>
#include "WAVWriter.h"

#define LED_PIN         22

// Connections to I2S microphone
#define I2S_WS          25
#define I2S_SD          33
#define I2S_SCK         32
 
// Use I2S Processor 0
#define I2S_PORT        I2S_NUM_0
 
// Define input buffers info
#define BUFF_LEN        1024
#define BUFF_COUNT      4

// Define sampling rate
#define SAMPLE_RATE     16000


/* Public class definition --------------------------- */
class Recorder
{
    public:
        Recorder(bool fixSPH0645 = false);

        void start(void);
        uint32_t read(int16_t *samples, uint32_t count);
        uint32_t getSampleRate(void);
        void stop(void);

    private:
        bool microphoneFix;
        const i2s_port_t i2sPort = I2S_PORT;
        const i2s_config_t i2sConfig = {
            .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
            .sample_rate = SAMPLE_RATE,
            .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
            .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = BUFF_COUNT,
            .dma_buf_len = BUFF_LEN,
            .use_apll = false, 
            .tx_desc_auto_clear = false,
            .fixed_mclk = 0
        };
        const i2s_pin_config_t i2sPins = {
            .bck_io_num = I2S_SCK,
            .ws_io_num = I2S_WS,
            .data_out_num = I2S_PIN_NO_CHANGE,
            .data_in_num = I2S_SD
        };

        void configureI2S(void);
};

void RecordSound(fs::FS &fs, Recorder *input, const char *fileName);

#endif /* RECORDER_H */
/* Sound recorder functions -------------------------------------*/
#include "recorder.h"

/* Private functions prototypes -------------------------------*/
/* Global variables -------------------------------------------*/
int16_t sBuffer[BUFF_LEN];

/**
  * @brief  Recorder class constructor
  * @param  None 
  * @retval None
  */
Recorder::Recorder()
{
    i2sInstall();
    i2sSetPin();
}

/**
  * @brief  Starting up sound recorder
  * @param  None
  * @retval None
  */
void Recorder::begin(void)
{
    i2s_start(I2S_PORT);
}

/**
  * @brief  Set up I2S processor configuration
  * @param  None
  * @retval None
  */
void Recorder::i2sInstall(void)
{
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = i2s_bits_per_sample_t(16),
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = BUFF_LEN,
    .use_apll = false
  };
 
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
}

/**
  * @brief  Set I2S pin configuration
  * @param  None
  * @retval None
  */
void Recorder::i2sSetPin(void)
{
      // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };
 
  i2s_set_pin(I2S_PORT, &pin_config);
}

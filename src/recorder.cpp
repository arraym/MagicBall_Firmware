/* Sound recorder functions -----------------------------------*/
#include "recorder.h"
#include "esp_log.h"

/* Private functions prototypes -------------------------------*/
/* Global variables -------------------------------------------*/
static const char *TAG = "WAV";

/**
  * @brief  Recorder class constructor
  * @param  None 
  * @retval None
  */
Recorder::Recorder(bool fixSPH0645)
{
  microphoneFix = fixSPH0645;
}

/**
  * @brief  Configures and starts I2S peripheral
  * @param  None
  * @retval None
  */
void Recorder::start(void)
{
  i2s_driver_install(i2sPort, &i2sConfig, 0, NULL);
  configureI2S();
}

/**
  * @brief  Reads samples from I2S peripheral
  * @param  samples - audio samples buffer pointer
  * @param  count - number of samples to read
  * @retval number of read samples
  */
uint32_t Recorder::read(int16_t *samples, uint32_t count)
{
  // read from i2s
  int32_t *rawSamples = (int32_t *)malloc(sizeof(int32_t) * count);
  size_t bytesRead = 0;

  i2s_read(i2sPort, rawSamples, sizeof(int32_t) * count, &bytesRead, portMAX_DELAY);
  int samplesRead = bytesRead / sizeof(int32_t);
  for (int i = 0; i < samplesRead; i++)
  {
    samples[i] = (rawSamples[i] & 0xFFFFFFF0) >> 11;
  }
  free(rawSamples);

  return samplesRead;
}

uint32_t Recorder::getSampleRate(void)
{
  return i2sConfig.sample_rate;
}

/**
  * @brief  Stops I2S peripheral
  * @param  None
  * @retval None
  */
void Recorder::stop(void)
{
  i2s_driver_uninstall(i2sPort);
}

/**
  * @brief  Configures and starts I2S peripheral
  * @param  None
  * @retval None
  */
void Recorder::configureI2S(void)
{
  if (microphoneFix)
  {
    // FIXES for SPH0645
    REG_SET_BIT(I2S_TIMING_REG(i2sPort), BIT(9));
    REG_SET_BIT(I2S_CONF_REG(i2sPort), I2S_RX_MSB_SHIFT);
  } 

  i2s_set_pin(i2sPort, &i2sPins);
}

/**
  * @brief  Records sound from I2S microphone
  * @param  fs - 
  * @param  input - 
  * @param  filename - 
  * @retval None
  */
void RecordSound(fs::FS &fs, Recorder *input, const char *fileName)
{
  uint8_t buffCounter = 0;

  int16_t *samples = (int16_t *)malloc(sizeof(int16_t) * BUFF_LEN);
  ESP_LOGI(TAG, "Start recording");
  input->start();
  // open the file on the sdcard
  File fp = fs.open(fileName, FILE_WRITE);
  // create a new wave file writer
  WAVWriter *writer = new WAVWriter(&fp, input->getSampleRate());

  while(buffCounter < 100)
  {
    int samplesRead = input->read(samples, BUFF_LEN);
    writer->write(samples, samplesRead);
    buffCounter++;
  }
  // stop the input
  input->stop();
  // and finish the writing
  writer->finish();
  fp.close();
  delete writer;
  free(samples);
  ESP_LOGI(TAG, "Finished recording");
}

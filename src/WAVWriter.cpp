/* WAVE file writer functions ---------------------------------*/
#include "WAVWriter.h"
#include "esp_log.h"

/* Private functions prototypes -------------------------------*/
/* Global variables -------------------------------------------*/
static const char *TAG = "WAV";

/**
  * @brief  WAVWriter class constructor
  * @param  fp - file pointer
  * @param  sampleRate - audio sample rate 
  * @retval None
  */
WAVWriter::WAVWriter(File *fp, uint32_t sampleRate)
{
  pFile = fp;
  fileHeader.sampleRate = sampleRate;

  // write out the header - we'll fill in some of the blanks later
  pFile->print((const char*)&fileHeader);
//   fwrite(&fileHeader, sizeof(WAV_HeaderTypeDef), 1, pFile);
  fileSize = sizeof(WAV_HeaderTypeDef);
}

/**
  * @brief  Writes samples to file
  * @param  samples - samples buffer pointer
  * @param  count - number of samples to write
  * @retval None
  */
void WAVWriter::write(int16_t *samples, uint32_t count)
{
  // write the samples and keep track of the file size so far
  pFile->print((const char*)&samples);
  fileSize += sizeof(int16_t) * count;
}

/**
  * @brief  Finishes wav file writing
  * @param  None
  * @retval None
  */
void WAVWriter::finish(void)
{
  ESP_LOGI(TAG, "Finishing wav file size: %d", fileSize);

  // now fill in the header with the correct information and write it again
  fileHeader.dataBytes = fileSize - sizeof(WAV_HeaderTypeDef);
  fileHeader.wavSize = fileSize - 8;
  pFile->seek(0);
  pFile->print((const char*)&fileHeader);
}

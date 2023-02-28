#ifndef WAVWRITER_H
#define WAVWRITER_H

#include <stdio.h>
#include "WAVFile.h"
#include "SD.h"
#include "FS.h"

/* Public class definition --------------------------- */
class WAVWriter
{
  public:
    WAVWriter(File *fp, uint32_t sampleRate);
    void write(int16_t *samples, uint32_t count);
    void finish(void);
  
  private:
    uint32_t fileSize;
    File *pFile;
    WAV_HeaderTypeDef fileHeader;
};

#endif /* WAVWRITER_H */
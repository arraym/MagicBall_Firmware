#ifndef WAVFILE_H
#define WAVFILE_H

#pragma pack(push, 1)
typedef struct wavHeader
{
  // RIFF Header
  char riffHeader[4];           // Contains "RIFF"
  int wavSize = 0;              // Size of the wav portion of the file, which follows the first 8 bytes. File size - 8
  char waveHeader[4];           // Contains "WAVE"

  // Format Header
  char fmtHeader[4];            // Contains "fmt " (includes trailing space)
  int fmtChunkSize = 16;        // Should be 16 for PCM
  short audioFormat = 1;        // Should be 1 for PCM. 3 for IEEE Float
  short numChannels = 2;
  int sampleRate = 22050;
  int byteRate = 88200;        // Number of bytes per second. sample_rate * num_channels * Bytes Per Sample
  short sampleAlignment = 4;   // num_channels * Bytes Per Sample
  short bitDepth = 16;         // Number of bits per sample

  // Data
  char dataHeader[4];          // Contains "data"
  int dataBytes = 0;           // Number of bytes in data. Number of samples * num_channels * sample byte size

  wavHeader()
  {
    riffHeader[0] = 'R';
    riffHeader[1] = 'I';
    riffHeader[2] = 'F';
    riffHeader[3] = 'F';
    waveHeader[0] = 'W';
    waveHeader[1] = 'A';
    waveHeader[2] = 'V';
    waveHeader[3] = 'E';
    fmtHeader[0]  = 'f';
    fmtHeader[1]  = 'm';
    fmtHeader[2]  = 't';
    fmtHeader[3]  = ' ';
    dataHeader[0] = 'd';
    dataHeader[1] = 'a';
    dataHeader[2] = 't';
    dataHeader[3] = 'a';
  }
} WAV_HeaderTypeDef;
#pragma pack(pop)

#endif /* WAVFILE_H */
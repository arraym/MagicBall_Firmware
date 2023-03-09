#include <Arduino.h>
#include <WiFi.h>
#include "recorder.h"
#include "storage.h"
#include "apserver.h"
#define USE_AUDIO_TOOLS

#ifdef USE_AUDIO_TOOLS
#include "AudioTools.h"
#endif

Recorder myRecorder;
Storage myStorage;
Apserver myServer;

extern WebServer server;

#ifdef USE_AUDIO_TOOLS
I2SStream i2sStream;                                              // Access I2S as stream
File audioFile;                                                   // final output stream
EncodedAudioStream out_stream(&audioFile, new WAVEncoder());      // encode as wav file
StreamCopy copier(out_stream, i2sStream);  

uint32_t bufferCounter = 0;
bool doneFlag = false;
#else
bool doneFlag = true;
#endif

void setup()
{
  Serial.begin(115200);
  // while(!Serial.available());    

  myStorage.begin();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH); 

#ifdef USE_AUDIO_TOOLS
  auto cfg = i2sStream.defaultConfig(RX_MODE);
  cfg.i2s_format = I2S_STD_FORMAT;
  cfg.sample_rate = SAMPLE_RATE;
  cfg.channels = 2;
  cfg.bits_per_sample = 16;
  i2sStream.begin(cfg);
  Serial.println("I2S started!");

  // we need to provide the audio information to the encoder
  out_stream.begin(cfg);
  // open the output file
  if(SD.exists("/Audio/test.wav"))
    SD.remove("/Audio/test.wav");
  audioFile = SD.open("/Audio/test.wav", "wb");
  if(!audioFile)
  {
    Serial.println("Failed to open file for writing");
    return;
  }

  delay(2000);
  digitalWrite(LED_PIN, LOW);
#else
  Serial.println(" ");
 
  // Start I2S recording
  Recorder *input = new Recorder();
  
  delay(500);
  digitalWrite(LED_PIN, LOW);
  RecordSound(SD, input, "/Audio/test.wav"); 
  digitalWrite(LED_PIN, HIGH);
  myServer.begin();
  myServer.hasStorage(myStorage.isMounted());
#endif
}

void loop()
{
  if(!doneFlag)
  {
#ifdef USE_AUDIO_TOOLS
    copier.copy();  
    if(bufferCounter > 2048)
    {
      doneFlag = true;
      audioFile.close();
      Serial.println("File writed successfuly!");
      digitalWrite(LED_PIN, HIGH);
      myServer.begin();
      myServer.hasStorage(myStorage.isMounted());
    }
    bufferCounter++;
#endif
  }
  else
  {
    myServer.clientHandle();
  }
}
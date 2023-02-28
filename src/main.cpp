#include <Arduino.h>
#include <WiFi.h>
#include "recorder.h"
#include "storage.h"
#include "apserver.h"

#include <WebServer.h>

#define LED_PIN   22

Recorder myRecorder;
Storage myStorage;
Apserver myServer;

extern WebServer server;

extern int16_t sBuffer[BUFF_LEN];

void setup()
{
    Serial.begin(115200);
    // while(!Serial.available());    

    myStorage.begin();

    // myStorage.listDir(SD, "/", 0);
    // myStorage.createDir(SD, "/mydir");
    // myStorage.listDir(SD, "/", 0);
    // myStorage.removeDir(SD, "/mydir");
    // myStorage.listDir(SD, "/", 2);
    // myStorage.writeFile(SD, "/hello.txt", "Hello ");
    // myStorage.appendFile(SD, "/hello.txt", "World!\n");
    // myStorage.readFile(SD, "/hello.txt");
    // myStorage.deleteFile(SD, "/foo.txt");
    // myStorage.renameFile(SD, "/hello.txt", "/foo.txt");
    // myStorage.readFile(SD, "/foo.txt");
    // myStorage.testFileIO(SD, "/test.txt");
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.println(" ");
 
  // Start I2S recording
  Recorder *input = new Recorder();
  
  delay(500);
  RecordSound(SD, input, "/Audio/test.wav");  

  myServer.begin();
  myServer.hasStorage(myStorage.isMounted());

  delay(1000);
}

void loop()
{
  myServer.clientHandle();
  
  // delay(100);

  // digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  // // False print statements to "lock range" on serial plotter display
  // // Change rangelimit value to adjust "sensitivity"
  // int rangelimit = 3000;
  // Serial.print(rangelimit * -1);
  // Serial.print(" ");
  // Serial.print(rangelimit);
  // Serial.print(" ");
 
  // // Get I2S data and place in data buffer
  // size_t bytesIn = 0;
  // esp_err_t result = i2s_read(I2S_PORT, &sBuffer, BUFF_LEN, &bytesIn, portMAX_DELAY);
 
  // if (result == ESP_OK)
  // {
  //   // Read I2S data buffer
  //   int16_t samples_read = bytesIn / 8;
  //   if (samples_read > 0) {
  //     float mean = 0;
  //     for (int16_t i = 0; i < samples_read; ++i) {
  //       mean += (sBuffer[i]);
  //     }
 
  //     // Average the data reading
  //     mean /= samples_read;
 
  //     // Print to serial plotter
  //     Serial.println(mean);
  //   }
  // }
}
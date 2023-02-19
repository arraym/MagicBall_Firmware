#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "recorder.h"
#include "storage.h"

#define LED_PIN   22

Recorder myRecorder;
Storage myStorage;

extern int16_t sBuffer[BUFF_LEN];

// Replace with your network credentials
const char* ssid     = "MagicBall";
const char* password = "14081986k";
const char* host     = "MagicBall";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String outputLedState = "off";

void setup()
{
    Serial.begin(115200);
    while(!Serial.available());

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

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  if (MDNS.begin(host)) {
    MDNS.addService("http", "tcp", 80);
    Serial.println("MDNS responder started");
    Serial.print("You can now connect to http://");
    Serial.print(host);
    Serial.println(".local");
  }

  server.begin();

  delay(1000);
 
  // Start I2S recording
  myRecorder.begin();
  
  delay(500);
  
}

void loop()
{
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /led/on") >= 0) {
              Serial.println("Blue LED on");
              outputLedState = "on";
              digitalWrite(LED_PIN, LOW);
            } else if (header.indexOf("GET /led/off") >= 0) {
              Serial.println("Blue LED off");
              outputLedState = "off";
              digitalWrite(LED_PIN, HIGH);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP32 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for blue LED 
            client.println("<p>Blue LED - State " + outputLedState + "</p>");
            // If the outputLedState is off, it displays the ON button       
            if (outputLedState=="off") {
              client.println("<p><a href=\"/led/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/led/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

//   digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  // False print statements to "lock range" on serial plotter display
  // Change rangelimit value to adjust "sensitivity"
//   int rangelimit = 3000;
//   Serial.print(rangelimit * -1);
//   Serial.print(" ");
//   Serial.print(rangelimit);
//   Serial.print(" ");
 
  // Get I2S data and place in data buffer
//   size_t bytesIn = 0;
//   esp_err_t result = i2s_read(I2S_PORT, &sBuffer, BUFF_LEN, &bytesIn, portMAX_DELAY);
 
//   if (result == ESP_OK)
//   {
//     // Read I2S data buffer
//     int16_t samples_read = bytesIn / 8;
//     if (samples_read > 0) {
//       float mean = 0;
//       for (int16_t i = 0; i < samples_read; ++i) {
//         mean += (sBuffer[i]);
//       }
 
//       // Average the data reading
//       mean /= samples_read;
 
//       // Print to serial plotter
//       Serial.println(mean);
//     }
//   }
}
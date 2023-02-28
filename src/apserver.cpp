/* Access point web server functions --------------------------*/
#include "apserver.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include "storage.h"

/* Private functions prototypes -------------------------------*/
void returnOK(void);
void returnFail(String msg);
bool loadFromSDCard(String path);
void handleFileUpload(void);
void deleteRecursive(String path);
void handleDelete(void);
void handleCreate(void);
void printDirectory(void);
void handleNotFound(void);

/* Global variables -------------------------------------------*/
const char* ssid     = "MagicBall";
const char* password = "14081986k";
const char* host     = "MagicBall";

WebServer server(80);
static bool hasSD;
File uploadFile;

/**
  * @brief  Web server class constructor
  * @param  None 
  * @retval None
  */
Apserver::Apserver()
{
  hasSD = false;
}

/**
  * @brief  Starting up server
  * @param  None
  * @retval None
  */
void Apserver::begin(void)
{
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  if(MDNS.begin(host))
  {
    MDNS.addService("http", "tcp", 80);
    Serial.println("MDNS responder started");
    Serial.print("You can now connect to http://");
    Serial.print(host);
    Serial.println(".local");
  }

  server.on("/list", HTTP_GET, printDirectory);
  server.on("/edit", HTTP_DELETE, handleDelete);
  server.on("/edit", HTTP_PUT, handleCreate);
  server.on("/edit", HTTP_POST, []() {
    returnOK();
  }, handleFileUpload);
  server.onNotFound(handleNotFound);

  server.begin();
}

void Apserver::hasStorage(bool storageFlag)
{
  hasSD = storageFlag;
}

void Apserver::clientHandle(void)
{
  server.handleClient();
}

void returnOK(void)
{
  server.send(200, "text/plain", "");
}

void returnFail(String msg)
{
  server.send(500, "text/plain", msg + "\r\n");
}

bool loadFromSDCard(String path)
{
  String dataType = "text/plain";
  if(path.endsWith("/"))
  {
    path += "index.htm";
  }

  if(path.endsWith(".src"))
  {
    path = path.substring(0, path.lastIndexOf("."));
  }
  else if(path.endsWith(".htm"))
  {
    dataType = "text/html";
  }
  else if(path.endsWith(".css"))
  {
    dataType = "text/css";
  }
  else if(path.endsWith(".js"))
  {
    dataType = "application/javascript";
  }
  else if(path.endsWith(".png"))
  {
    dataType = "image/png";
  }
  else if(path.endsWith(".gif"))
  {
    dataType = "image/gif";
  }
  else if(path.endsWith(".jpg"))
  {
    dataType = "image/jpeg";
  }  
  else if(path.endsWith(".ico"))
  {
    dataType = "image/x-icon";
  }
  else if(path.endsWith(".xml"))
  {
    dataType = "text/xml";
  }
  else if(path.endsWith(".pdf"))
  {
    dataType = "application/pdf";
  }
  else if(path.endsWith(".zip"))
  {
    dataType = "application/zip";
  }

  File dataFile = SD.open(path.c_str());
  if(dataFile.isDirectory())
  {
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if(!dataFile)
  {
    return false;
  }

  if(server.hasArg("download"))
  {
    dataType = "application/octet-stream";
  }

  if(server.streamFile(dataFile, dataType) != dataFile.size())
  {
    Serial.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

void handleFileUpload(void)
{
  if(server.uri() != "/edit")
  {
    return;
  }
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START)
  {
    if(SD.exists((char *)upload.filename.c_str()))
    {
      SD.remove((char *)upload.filename.c_str());
    }
    uploadFile = SD.open(upload.filename.c_str(), FILE_WRITE);
    Serial.print("Upload: START, filename: "); 
    Serial.println(upload.filename);
  }
  else if(upload.status == UPLOAD_FILE_WRITE)
  {
    if(uploadFile)
    {
      uploadFile.write(upload.buf, upload.currentSize);
    }
    Serial.print("Upload: WRITE, Bytes: ");
    Serial.println(upload.currentSize);
  } 
  else if(upload.status == UPLOAD_FILE_END)
  {
    if(uploadFile)
    {
      uploadFile.close();
    }
    Serial.print("Upload: END, Size: ");
    Serial.println(upload.totalSize);
  }
}

void deleteRecursive(String path)
{
  File file = SD.open((char *)path.c_str());
  if(!file.isDirectory())
  {
    file.close();
    SD.remove((char *)path.c_str());
    return;
  }

  file.rewindDirectory();
  while(true)
  {
    File entry = file.openNextFile();
    if(!entry)
    {
      break;
    }
    String entryPath = path + "/" + entry.name();
    if(entry.isDirectory())
    {
      entry.close();
      deleteRecursive(entryPath);
    }
    else
    {
      entry.close();
      SD.remove((char *)entryPath.c_str());
    }
    yield();
  }

  SD.rmdir((char *)path.c_str());
  file.close();
}

void handleDelete(void)
{
  if(server.args() == 0)
  {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  if(path == "/" || !SD.exists((char *)path.c_str()))
  {
    returnFail("BAD PATH");
    return;
  }
  deleteRecursive(path);
  returnOK();
}

void handleCreate(void)
{
  if(server.args() == 0)
  {
    return returnFail("BAD ARGS");
  }
  String path = server.arg(0);
  if(path == "/" || SD.exists((char *)path.c_str()))
  {
    returnFail("BAD PATH");
    return;
  }

  if(path.indexOf('.') > 0)
  {
    File file = SD.open((char *)path.c_str(), FILE_WRITE);
    if(file)
    {
      file.write(0);
      file.close();
    }
  }
  else
  {
    SD.mkdir((char *)path.c_str());
  }
  returnOK();
}

void printDirectory(void)
{
  if(!server.hasArg("dir"))
  {
    return returnFail("BAD ARGS");
  }
  String path = server.arg("dir");
  if(path != "/" && !SD.exists((char *)path.c_str()))
  {
    return returnFail("BAD PATH");
  }
  File dir = SD.open((char *)path.c_str());
  path = String();
  if(!dir.isDirectory())
  {
    dir.close();
    return returnFail("NOT DIR");
  }
  dir.rewindDirectory();
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  // server.send(200, "text/json", "");
  server.send(200, "text/html", "");
  WiFiClient client = server.client();

  // server.sendContent("[");
  server.sendContent("<!DOCTYPE html><html><body>");
  for(int cnt = 0; true; ++cnt)
  {
    File entry = dir.openNextFile();
    if(!entry)
    {
      break;
    }

    String output;
    if(cnt > 0)
    {
      // output = ',';
      output = "<br>";
    }

    output += "<a href=\"";
    output += entry.path();
    output += "\">";
    output += entry.path();
    output += "</a> - size: ";
    output += entry.size();
    output += " bytes";
    
    // output += "{\"type\":\"";
    // output += (entry.isDirectory()) ? "dir" : "file";
    // output += "\",\"name\":\"";
    // output += entry.path();
    // output += "\"";
    // output += "}";
    server.sendContent(output);
    entry.close();
  }
  // server.sendContent("]");
  server.sendContent("</body></html>");
  dir.close();
}

void handleNotFound(void)
{
  if(hasSD && loadFromSDCard(server.uri()))
  {
    return;
  }
  String message = "SDCARD Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for(uint8_t i = 0; i < server.args(); i++)
  {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.print(message);
}

/* Storage functions -------------------------------------*/
#include "storage.h"
#include "SPI.h"

/* Private functions prototypes -------------------------------*/
/* Global variables -------------------------------------------*/

/**
  * @brief  Storage class constructor
  * @param  None 
  * @retval None
  */
Storage::Storage()
{
    mountFlag = SD.begin(CS_PIN);

    if(mountFlag)
        cardType = SD.cardType();
}

/**
  * @brief  Starting up storage
  * @param  None
  * @retval None
  */
void Storage::begin(void)
{
    if(!mountFlag){
        Serial.println("Card Mount Failed");
        return;
    }

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

/**
  * @brief  Lists directories
  * @param  fs - file sistem reference
  * @param  dirname - directory name
  * @param  levels - depth level
  * @retval None
  */
void Storage::listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root)
    {
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory())
    {
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file)
    {
        if(file.isDirectory())
        {
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels)
            {
                listDir(fs, file.name(), levels -1);
            }
        }
        else
        {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

/**
  * @brief  Creates a directory
  * @param  fs - file sistem reference
  * @param  path - directory path
  * @retval None
  */
void Storage::createDir(fs::FS &fs, const char *path)
{
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path))
    {
        Serial.println("Dir created");
    }
    else
    {
        Serial.println("mkdir failed");
    }
}

/**
  * @brief  Removes a directory
  * @param  fs - file sistem reference
  * @param  path - directory path
  * @retval None
  */
void Storage::removeDir(fs::FS &fs, const char *path)
{
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path))
    {
        Serial.println("Dir removed");
    }
    else
    {
        Serial.println("rmdir failed");
    }
}

/**
  * @brief  Reads from file
  * @param  fs - file sistem reference
  * @param  path - file path
  * @retval None
  */
void Storage::readFile(fs::FS &fs, const char *path)
{
    Serial.printf("Reading file: %s\n", path);
    File file = fs.open(path);
    if(!file)
    {
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available())
    {
        Serial.write(file.read());
    }
    file.close();
}

/**
  * @brief  Writes to file
  * @param  fs - file sistem reference
  * @param  path - file path
  * @param  message - message to be write in file
  * @retval None
  */
void Storage::writeFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Writing file: %s\n", path);
    File file = fs.open(path, FILE_WRITE);
    if(!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message))
    {
        Serial.println("File written");
    }
    else
    {
        Serial.println("Write failed");
    }
    file.close();
}

/**
  * @brief  Appends to file content
  * @param  fs - file sistem reference
  * @param  path - file path
  * @param  message - message to be append in file
  * @retval None
  */
void Storage::appendFile(fs::FS &fs, const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\n", path);
    File file = fs.open(path, FILE_APPEND);
    if(!file)
    {
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message))
    {
        Serial.println("Message appended");
    }
    else
    {
        Serial.println("Append failed");
    }
    file.close();
}

/**
  * @brief  Renames a file
  * @param  fs - file sistem reference
  * @param  oldPath - file path
  * @param  newPath - new file path
  * @retval None
  */
void Storage::renameFile(fs::FS &fs, const char *oldPath, const char *newPath)
{
    Serial.printf("Renaming file %s to %s\n", oldPath, newPath);
    if (fs.rename(oldPath, newPath))
    {
        Serial.println("File renamed");
    }
    else
    {
        Serial.println("Rename failed");
    }
}

/**
  * @brief  Deletes a file
  * @param  fs - file sistem reference
  * @param  path - file path
  * @retval None
  */
void Storage::deleteFile(fs::FS &fs, const char *path)
{
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path))
    {
        Serial.println("File deleted");
    }
    else
    {
        Serial.println("Delete failed");
    }
}

/**
  * @brief  Test class functionalities
  * @param  fs - file sistem reference
  * @param  path - file path
  * @retval None
  */
void Storage::testFileIO(fs::FS &fs, const char *path)
{
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file)
    {
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len)
        {
            size_t toRead = len;
            if(toRead > 512)
            {
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    }
    else
    {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file)
    {
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i = 0; i < 2048; i++)
    {
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

#ifndef STORAGE_H
#define STORAGE_H

#include <stdint.h>
#include "SD.h"
#include "FS.h"

#define CS_PIN          5

/* Public class definition --------------------------- */
class Storage
{
    public:
        Storage();
        ~Storage() = default;

        void begin(void);
        void listDir(fs::FS &fs, const char * dirname, uint8_t levels);
        void createDir(fs::FS &fs, const char * path);
        void removeDir(fs::FS &fs, const char * path);
        void readFile(fs::FS &fs, const char * path);
        void writeFile(fs::FS &fs, const char * path, const char * message);
        void appendFile(fs::FS &fs, const char * path, const char * message);
        void renameFile(fs::FS &fs, const char * path1, const char * path2);
        void deleteFile(fs::FS &fs, const char * path);
        void testFileIO(fs::FS &fs, const char * path);
        bool isMounted(void);

    private:
        bool mountFlag;
        uint8_t cardType;
};

#endif /* STORAGE_H */
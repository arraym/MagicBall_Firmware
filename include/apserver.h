#ifndef APSERVER_H
#define APSERVER_H

#include <stdint.h>
#include <WString.h>

/* Public class definition --------------------------- */
class Apserver
{
    public:
        Apserver();
        ~Apserver() = default;

        void begin(void);
        void hasStorage(bool storageFlag);
        void clientHandle(void);        
};

#endif /* ApSERVER_H */
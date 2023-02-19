#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <stdint.h>

/* Public class definition --------------------------- */
class Webserver
{
    public:
        Webserver();
        ~Webserver() = default;

        void begin(void);
};

#endif /* WEBSERVER_H */
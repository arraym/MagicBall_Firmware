#ifndef RECORDER_H
#define RECORDER_H

#include <stdint.h>
#include <driver/i2s.h>

// Connections to I2S microphone
#define I2S_WS          25
#define I2S_SD          33
#define I2S_SCK         32
 
// Use I2S Processor 0
#define I2S_PORT        I2S_NUM_0
 
// Define input buffer length
#define BUFF_LEN        64

/* Public class definition --------------------------- */
class Recorder
{
    public:
        Recorder();
        ~Recorder() = default;

        void begin(void);

    private:
        void i2sInstall(void);
        void i2sSetPin(void);
};

#endif /* RECORDER_H */
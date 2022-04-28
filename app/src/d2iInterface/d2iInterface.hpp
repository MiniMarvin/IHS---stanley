#ifndef __D2I_INTERFACE
#define __D2I_INTERFACE

#include <stdint.h>	/* uints types */

enum D2iDevice {
    RED_LEDS,
    GREEN_LEDS,
    SWITCHES,
    PUSH_BUTTONS,
    DISPLAY_RIGHT,
    DISPLAY_LEFT
};

class D2iInterface {
private:
    int fileDescriptor;
public:
    D2iInterface(char* driverPath);
    ~D2iInterface();
    
    void write(uint32_t bytes, D2iDevice device);
    uint32_t read(uint32_t bytes, D2iDevice device);
    
    void leftDisplayWrite(int num);
    void rightDisplayWrite(int num);
    
    uint32_t readButtons();
    bool readButton(int index);
    
    uint32_t readSwitches();
    bool readSwitch(int index);
    
    void writeGreenLeds(uint32_t bits);
    void writeGreenLed(bool value, int index);
    void writeRedLeds(uint32_t bits);
    void writeRedLed(bool value, int index);
};

#endif

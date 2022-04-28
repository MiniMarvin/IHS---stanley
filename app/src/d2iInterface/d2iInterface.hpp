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
    
    int writeValue(unsigned int data, D2iDevice device);
    unsigned int readValue(unsigned int bytes, D2iDevice device);
    
    void leftDisplayWrite(int num);
    void rightDisplayWrite(int num);
    
    unsigned int readButtons();
    bool readButton(int index);
    
    unsigned int readSwitches();
    bool readSwitch(int index);
    
    void writeGreenLeds(unsigned int bits);
    void writeGreenLed(bool value, int index);
    void writeRedLeds(unsigned int bits);
    void writeRedLed(bool value, int index);
};

#endif

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
    const int displayMap[16] = {0xC0, 0xF9, 0xA4, 0xB0, 
        0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0x88, 
        0x83, 0xC6, 0xA1, 0x86, 0x8E};
    int fileDescriptor;
    unsigned int greenLedsState = 0;
    unsigned int redLedsState = 0;
    unsigned int printSingleDisplayNum(int num);
    unsigned int printDisplayNum(int num);
public:
    D2iInterface(char* driverPath);
    ~D2iInterface();
    
    int writeValue(unsigned int data, D2iDevice device);
    unsigned int readValue(unsigned int bytes, D2iDevice device);
    
    int leftDisplayWrite(int num);
    int rightDisplayWrite(int num);
    
    unsigned int readButtons();
    bool readButton(int index);
    
    unsigned int readSwitches();
    bool readSwitch(int index);
    
    int writeGreenLeds(unsigned int data);
    int writeGreenLed(bool value, int index);
    int writeRedLeds(unsigned int data);
    int writeRedLed(bool value, int index);
};

#endif

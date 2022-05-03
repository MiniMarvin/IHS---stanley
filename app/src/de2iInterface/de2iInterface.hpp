#ifndef __DE2I_INTERFACE
#define __DE2I_INTERFACE

#include <stdint.h>	/* uints types */

enum De2iDevice {
    RED_LEDS,
    GREEN_LEDS,
    SWITCHES,
    PUSH_BUTTONS,
    DISPLAY_RIGHT,
    DISPLAY_LEFT
};

typedef struct PerifericValues {
    unsigned int displayLeft;
    unsigned int displayRight;
    unsigned int redLeds;
    unsigned int greenLeds;
    PerifericValues(): displayLeft(0), displayRight(0), redLeds(0), greenLeds(0) {}
} PerifericValues;

class De2iInterface {
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
    De2iInterface(char* driverPath);
    ~De2iInterface();
    
    int writeValue(unsigned int data, De2iDevice device);
    unsigned int readValue(unsigned int bytes, De2iDevice device);
    
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

#include "echo.hpp"
#include <stdio.h>	/* printf */
#include <stdlib.h>	/* malloc, atoi, rand... */
#include <string.h>	/* memcpy, strlen... */
#include <stdint.h>	/* uints types */
#include <sys/types.h>	/* size_t ,ssize_t, off_t... */
#include <unistd.h>	/* close() read() write() */
#include <fcntl.h>	/* open() */
#include <sys/ioctl.h>	/* ioctl() */
#include <errno.h>	/* error codes */
#include <iostream>
#include <bitset>

#include "../de2iInterface/de2iInterface.hpp"

using namespace std;

int echoLoop(char* driverPath) {
    cout << endl;
    cout << "Entering echo loop..." <<endl;
    
    De2iInterface interface = De2iInterface(driverPath);
    
    while(1) {
        cout << "Reading switches..." << endl;
        unsigned int switches = interface.readSwitches();
        cout << "Did read the hex value: " << hex << switches << endl;
        bitset<8> binarySwitches(switches);
        cout << "Did read the bin value: " << binarySwitches << endl;
        
        interface.writeRedLeds(switches);
    
        cout << "Reading buttons..." << endl;
        unsigned int buttons = interface.readButtons();
        cout << "Did read the hex value: " << hex << buttons << endl;
        bitset<8> binaryButtos(buttons);
        cout << "Did read the bin value: " << binaryButtos << endl;
        
        interface.writeGreenLeds(buttons);
    }
    
	return 0;
}
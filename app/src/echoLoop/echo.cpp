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
        unsigned int switches = interface.readSwitches();
        unsigned int buttons = interface.readButtons();
        interface.leftDisplayWrite(switches/1000);
        interface.rightDisplayWrite(switches%1000);
        interface.writeRedLeds(switches);
        interface.writeGreenLeds((buttons & 0b1111) ^ 0b1111);
        usleep(100000);
    }
    
	return 0;
}

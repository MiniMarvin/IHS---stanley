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
    cout << "Reading switches..." << endl;
    unsigned int switches = interface.readSwitches();
    cout << "Did read the hex value: " << hex << switches << endl;
    bitset<8> x(switches);
    cout << "Did read the bin value: " << x << endl;
    
    
    unsigned int switch1 = interface.readSwitch(1);
    cout << "Did read bit on switch1: " << switch1 << endl;
    
    unsigned int switch2 = interface.readSwitch(2);
    cout << "Did read bit on switch2: " << switch2 << endl;
    
	return 0;
}
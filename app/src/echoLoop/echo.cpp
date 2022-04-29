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

#include "../d2iInterface/d2iInterface.hpp"

using namespace std;

int echoLoop(char* driverPath) {
    cout << endl;
    cout << "Entering echo loop..." <<endl;
    
    D2iInterface interface = D2iInterface(driverPath);
// 	interface.readValue
	
	return 0;
}
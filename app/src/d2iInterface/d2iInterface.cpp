#include "d2iInterface.hpp"
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

using namespace std;

D2iInterface::D2iInterface(char* driverPath) {
    int fileDescriptor = 0;
    if ((fileDescriptor = open(driverPath, O_RDWR)) < 0) {
		fprintf(stderr, "Error opening file %s\n", driverPath);
	}
	cout << "Driver file " << driverPath << " loaded..." << endl;
}
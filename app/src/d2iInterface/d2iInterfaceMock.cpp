// when want to use mock just use this directive
// #define __D2I_INTERFACE_MOCK
#ifdef __D2I_INTERFACE_MOCK

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

#include "../exceptions/DriverException.hpp"
#include "ioctl_cmds.h"


D2iInterface::D2iInterface(char* driverPath) {
    cout << "Using Mock Interface" << endl;
	cout << "Driver file " << driverPath << " loaded..." << endl;
	this->fileDescriptor = 0;
}

D2iInterface::~D2iInterface(){}

int D2iInterface::writeValue(unsigned int data, D2iDevice device) {
	return 0;
}

unsigned int D2iInterface::readValue(unsigned int bytes, D2iDevice device) {
	return 0;
}

int D2iInterface::leftDisplayWrite(int num) {
	return 0;
}

int D2iInterface::rightDisplayWrite(int num) {
	return 0;
}

unsigned int D2iInterface::printSingleDisplayNum(int num) {
	return 0;
}

unsigned int D2iInterface::printDisplayNum(int num) {
	return 0;
}

#endif
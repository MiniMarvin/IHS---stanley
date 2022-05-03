// when want to use mock just use this directive
// #define __DE2I_INTERFACE_MOCK
#ifdef __DE2I_INTERFACE_MOCK

#include "de2iInterface.hpp"
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

De2iInterface::De2iInterface(char* driverPath) {
    cout << "Using Mock Interface" << endl;
	cout << "Driver file " << driverPath << " loaded..." << endl;
	this->fileDescriptor = 0;
}

De2iInterface::~De2iInterface(){}

int De2iInterface::writeValue(unsigned int data, De2iDevice device) {
	return 0;
}

unsigned int De2iInterface::readValue(unsigned int bytes, De2iDevice device) {
	return 0;
}

int De2iInterface::leftDisplayWrite(int num) {
	return 0;
}

int De2iInterface::rightDisplayWrite(int num) {
	return 0;
}

unsigned int De2iInterface::printSingleDisplayNum(int num) {
	return 0;
}

unsigned int De2iInterface::printDisplayNum(int num) {
	return 0;
}

#endif
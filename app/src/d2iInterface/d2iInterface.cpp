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

using namespace std;

D2iInterface::D2iInterface(char* driverPath) {
    int fileDescriptor = 0;
    if ((fileDescriptor = open(driverPath, O_RDWR)) < 0) {
// 		fprintf(stderr, "Error opening file %s\n", driverPath);
		throw(DriverException("Error opening driver file"));
	}
	cout << "Driver file " << driverPath << " loaded..." << endl;
	this->fileDescriptor = fileDescriptor;
}

D2iInterface::~D2iInterface(){}

int D2iInterface::writeValue(unsigned int data, D2iDevice device) {
	unsigned long operation = 0;
	if(device == RED_LEDS) {
		operation = WR_RED_LEDS;
	}
	else if (device == GREEN_LEDS) {
		operation = WR_GREEN_LEDS;
	}
	else if (device == DISPLAY_RIGHT) {
		operation = WR_R_DISPLAY;
	}
	else if (device == DISPLAY_LEFT) {
		operation = WR_L_DISPLAY;
	}
	else {
		return -1;
	}
	
	ioctl(this->fileDescriptor, operation);
	int retval = write(fileDescriptor, &data, sizeof(data));
	return retval;
}

unsigned int D2iInterface::readValue(unsigned int bytes, D2iDevice device) {
	unsigned long operation = 0;
	if(device == SWITCHES) {
		operation = RD_SWITCHES;
	}
	else if (device == PUSH_BUTTONS) {
		operation = RD_PBUTTONS;
	}
	else {
		return -1;
	}
	
	uint32_t data = 0;
	ioctl(this->fileDescriptor, operation);
	read(fileDescriptor, &data, bytes);
	return data;
}
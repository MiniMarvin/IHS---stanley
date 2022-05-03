// when want to use mock just use this directive
// #define __DE2I_INTERFACE_MOCK
#ifndef __DE2I_INTERFACE_MOCK

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
#include <omp.h>

using namespace std;

// =========================================================
// Interface with the board
// =========================================================
De2iInterface::De2iInterface(char* driverPath) {
    int fileDescriptor = 0;
    if ((fileDescriptor = open(driverPath, O_RDWR)) < 0) {
		throw(DriverException("Error opening driver file"));
	}
	cout << "Driver file " << driverPath << " loaded..." << endl;
	this->fileDescriptor = fileDescriptor;
}

De2iInterface::~De2iInterface(){}

int De2iInterface::writeValue(unsigned int data, De2iDevice device) {
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

unsigned int De2iInterface::readValue(unsigned int bytes, De2iDevice device) {
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

int De2iInterface::leftDisplayWrite(int num) {
	unsigned int displayData = this->printDisplayNum(num);
	return this->writeValue(displayData, DISPLAY_LEFT);
}

int De2iInterface::rightDisplayWrite(int num) {
	unsigned int displayData = this->printDisplayNum(num);
	return this->writeValue(displayData, DISPLAY_RIGHT);
}

unsigned int De2iInterface::printSingleDisplayNum(int num) {
	if (num < 0 || num > 15) return 0;
	return this->displayMap[num];
}

unsigned int De2iInterface::printDisplayNum(int num) {
	int d0 = num%10;
	int d1 = (num/10)%10;
	int d2 = (num/100)%10;
	int d3 = (num/1000)%10;
	
	unsigned int displayData0 = this->printSingleDisplayNum(d0);
	unsigned int displayData1 = this->printSingleDisplayNum(d1);
	unsigned int displayData2 = this->printSingleDisplayNum(d2);
	unsigned int displayData3 = this->printSingleDisplayNum(d3);
	unsigned int displayData = (((displayData3 << 24) |
		(displayData2 << 16) | displayData1 << 8) | displayData0);
	return displayData;
}

unsigned int De2iInterface::readButtons() {
	return (0xff & ~this->readValue(4, PUSH_BUTTONS));
}

bool De2iInterface::readButton(int index) {
	unsigned int buttons = readButtons();
	bool status = (buttons & (1 << index)) != 0;
	return status;
}

unsigned int De2iInterface::readSwitches() {
	return this->readValue(4, SWITCHES);
}

bool De2iInterface::readSwitch(int index) {
	unsigned int switches = readSwitches();
	bool status = (switches & (1 << index)) != 0;
	return status;
}

int De2iInterface::writeGreenLeds(unsigned int data) {
	int status = this->writeValue(data, GREEN_LEDS);
	if (status > 0) {
		this->greenLedsState = data;
	}
	return status;
}

int De2iInterface::writeGreenLed(bool value, int index) {
	unsigned int data = ((this->greenLedsState & (1 << index)) | 
							(value << index));
	int status = this->writeValue(data, GREEN_LEDS);
	if (status > 0) {
		this->greenLedsState = data;
	}
	return status;
}

int De2iInterface::writeRedLeds(unsigned int data) {
	int status = this->writeValue(data, RED_LEDS);
	if (status > 0) {
		this->redLedsState = data;
	}
	return status;
}

int De2iInterface::writeRedLed(bool value, int index) {
	unsigned int data = ((this->redLedsState & (1 << index)) | 
							(value << index));
	int status = this->writeValue(data, RED_LEDS);
	if (status > 0) {
		this->redLedsState = data;
	}
	return status;
}

#endif
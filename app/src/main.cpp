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

#include "ioctl_cmds.h"
#include "./echoLoop/echo.hpp"

#define GAME_LOOP 0
#define ECHO_LOOP 1

using namespace std;

char* driverPath = (char*)"/dev/stanley_pci";

int configGame(int, char**);
void configDriverPath(int, char**);
int configGameLoop(int, char**);

int main(int argc, char** argv) {
	cout << "Init software..." << endl;
	cout << "Configure game..." << endl;
	int config = configGame(argc, argv);
	int loop = 0;
	if (config == ECHO_LOOP) {
		cout << "Using echo loop" << endl;
		loop = echoLoop(driverPath);
	} else {
		// add game loop
	}
	
	if (loop != 0) {
		cout << "There was an error on the game loop" << endl;
	}
	return 0;
}

int configGame(int argc, char** argv) {
	configDriverPath(argc, argv);
	return configGameLoop(argc, argv);
}

void configDriverPath(int argc, char** argv) {
	if (argc >= 2) {
		cout << "Using custom file" << argv[1] << endl;
		driverPath = (char*) malloc(1000);
		strcpy(driverPath, argv[1]);
	}
	cout << "Loaded driver file: " << driverPath << endl;
}

int configGameLoop(int argc, char** argv) {
	int loopType = GAME_LOOP;
	
	if (argc >= 3) {
		if (!strcmp("echo", argv[2])) {
			loopType = ECHO_LOOP;
		}
	}
	
	cout << "Using the " << (loopType == ECHO_LOOP ? "Echo" : "Game") << " Loop" << endl;
	
	return loopType;
}
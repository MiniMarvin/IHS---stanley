#ifndef __GAME_LOOP
#define __GAME_LOOP

#include <iostream>
#include <bitset>
#include <vector>
#include <tuple>
#include <omp.h>
#include <chrono>

#include "../de2iInterface/de2iInterface.hpp"
#include "timer/timer.hpp"

using namespace std;

enum GamePhase {
    IntroPhase,
    ButtonPhase,
    SwitchPhase,
    EndgamePhase
};

// Game Management
int gameLoop(char*);
GamePhase gameOperation(GamePhase phase, De2iInterface interface, PerifericValues& periferics);
bool runGreenLedsAndPushButtonsGameAndCheckIfWin(int numberOfLeds, De2iInterface interface);
bool runRedLedsAndSwitchesAndCheckIfWin(De2iInterface interface);
void updatePeriferals(PerifericValues& periferics, De2iInterface interface, TimePoint startTime);

// Game Utils
vector<int> getOrderOfGreenLeds(int count);
pair<int, int> translateButtonToGreenLed(int position);
void lightUpAndLightDownGreenLed(pair<int, int> positions, De2iInterface interface);
void lightUpGreenLightFromVector(vector<int> array, De2iInterface interface);
bool checkIfPositionOfButtonIsEquivalentOfGreenLight(pair<int, int> positionsOsLedsTranslated, int positionOfGreenLight);
void showPoints(De2iInterface interface);
void updatePoints(De2iInterface interface);
void resetPoints(De2iInterface interface);


// General utils
int isPowerOfTwo(unsigned n);
int findPosition(unsigned n);
void printArray(vector<int> array);
void configOmp(int threadCount);

#endif
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
#include "periferics/Perifericvalues.hpp"

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
bool runGreenLedsAndPushButtonsGameAndCheckIfWin(int numberOfLeds, De2iInterface interface, PerifericValues periferics);
bool runRedLedsAndSwitchesAndCheckIfWin(De2iInterface interface);
void updatePeriferals(PerifericValues& periferics, De2iInterface interface, TimePoint startTime);

// Game Utils
vector<int> getOrderOfGreenLeds(int count);
pair<int, int> translateButtonToGreenLed(int position);
void lightUpAndLightDownGreenLed(pair<int, int> positions, PerifericValues periferics);
void lightUpGreenLightFromVector(vector<int> array, PerifericValues periferics);
bool checkIfPositionOfButtonIsEquivalentOfGreenLight(pair<int, int> positionsOsLedsTranslated, int positionOfGreenLight);
void showPoints(PerifericValues periferics);
void updatePoints(PerifericValues periferics);
void resetPoints(PerifericValues periferics);
void lostGame(PerifericValues& periferics, Timer& timer);

// General utils
int isPowerOfTwo(unsigned n);
int findPosition(unsigned n);
void printArray(vector<int> array);
void configOmp(int threadCount);

#endif
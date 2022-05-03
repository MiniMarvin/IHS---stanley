#include "game.hpp"
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
#include <vector>
#include <tuple>
#include <omp.h>
#include <chrono>

#include "../de2iInterface/de2iInterface.hpp"
#include "game.hpp"
#include "timer/timer.hpp"

using namespace std;

// ============================================================
// Types & Enums
// ============================================================
const int BUTTON_COUNT = 4;
int TOTAL_POINTS = 0;

// ============================================================
// Game Loop
// ============================================================
int gameLoop(char* driverPath) {
    cout << endl;
    cout << "Iniciando jogo..." <<endl;
    
    De2iInterface interface = De2iInterface(driverPath);
    GamePhase phase = ButtonPhase;
    
    // TODO: add openMP to split in threads use SECTIONS
    while (1) {
        phase = gameOperation(phase, interface);
    }
    
	return 0;
}

// ===========================================================
// Dividir as fases de maneira incremental, o jogo vai ser basicamente dividido em algumas partes:
// 1. Inicio: Exibe uma tela de loading, pode ser qualquer coisa, como um shift de LED vermelho até a pessoa
// apertar start e dar inicio ao programa
// 2. Fase Botão: Vai Iniciar o Jogo exibindo para o usuário a sequência de botões para apertar
// 3. Fase Botão Entrada: Vai receber do usuário um input para poder comparar com o que o usuário digitou
// 4. Fase Switch: Vai mostrar ao usuário uma sequência de bits ON e OFF que o usuário precisa colocar e um timer
// daí o usuário precisa colocar essa fase antes do timer acabar
// 5. Fase de pontos: Após o usuário perder vai ser exibida a pontuação que ele fez e apertando start de novo
// O usuário volta para a fase 1
// -----------------------------------------------------------
// OBS: Essa thread não deveria escrever diretamente na interface e sim ler dela, quem deveria escrever é a outra thread
// que vai atualizar a tela a 60 FPS.
GamePhase gameOperation(GamePhase phase, De2iInterface interface) {
    GamePhase newPhase = phase;
    
    switch (phase) {
        case IntroPhase: {
            break;
        }
        case ButtonPhase: {
            // TODO: fazer essas fases serem incrementais e o jogo ser no mínimo um jogo infinito
            // Primeira fase: acende aleatoriamente 5 valores de leds verde e compara com os botões apertados
            bool win = runGreenLedsAndPushButtonsGameAndCheckIfWin(5, interface);
            if(win) newPhase = SwitchPhase;
            else newPhase = IntroPhase;
            break;
        }
        case SwitchPhase: {
            //bool win = runRedLedAndTurnSwitchGameAndCheckIfWin(10, interface);
            //if(win) newPhase = EndgamePhase;
            break;
        }
        case EndgamePhase: {
            newPhase = lostGame(interface);
            break;
        }
    }
    
    return newPhase;
}


bool runGreenLedsAndPushButtonsGameAndCheckIfWin(int roundCount, De2iInterface interface) {
    cout << "iniciando operação de LEDs" << endl;
    vector<int> orderToLightUp = getOrderOfGreenLeds(roundCount);
    lightUpGreenLightFromVector(orderToLightUp, interface);
    
    cout << "Esperando botões..." << endl;
    unsigned int LED_INDEX = 0;
    unsigned int prevValue = 15;
    
    // TODO: add time 
    // Loop para esperar interações do usuário
    while(1) {
        // TODO: handle line up and line down
        unsigned int buttons = interface.readButtons();
        cout << "Valor lido: " << buttons << endl;
        if (buttons == prevValue) {
            cout << "ignoring because of selected values"<< endl;
            continue;
        }
        prevValue = buttons;
        
        // Pega a posição do botão apertado; retorna -1 se nenhum ou mais de um forem apertados ao mesmo tempo.
        int positionOfButtonClicked = findPosition(buttons);
        cout << "Posição do botão clicado: " << positionOfButtonClicked << endl;
    
        pair<int, int> ledsToLightUp = translateButtonToGreenLed(positionOfButtonClicked);
        
        // Se a posição for igual a esperada
        if(checkIfPositionOfButtonIsEquivalentOfGreenLight(ledsToLightUp, orderToLightUp[LED_INDEX])) {
            lightUpAndLightDownGreenLed(ledsToLightUp, interface);
            
            // Se a posição for a última, vai para a próxima fase
            if(LED_INDEX == orderToLightUp.size() - 1) {
                cout << "Parabéns! Seguindo para o segundo nível!" << endl;
                return true;
            }
            
            LED_INDEX++;
            updatePoints(interface);
        } else {
            return false;
        }
        
        sleep(1);
    }
}

vector<int> getOrderOfGreenLeds(int count) {
    vector<int> orderToLightUp;
    
    for(int i = 0; i < count; i++) {
        int value = rand()%4; // 4 positions (0 to 3)
        orderToLightUp.push_back(value);
    }

    printArray(orderToLightUp);
    
    return orderToLightUp;
}


void printArray(vector<int> array) {
    cout << "Posições para ligar LEDs: ";
    
    for(unsigned int i = 0; i < array.size(); i++){
        cout << array[i] << " ";
    }
    
    cout << endl;
}

// Vamos só ignorar os leds de 7 a 4 porque não precisa
// 7 6 5 4 3 2 1 0 --- índice leds
// l l l l l l l l --- leds
// ( ) ( ) ( ) ( ) --- botões
//  3   2   1   0  --- índice botões
pair<int, int> translateButtonToGreenLed(int position) {
    int firstLed = position*2;
    int secondLed = firstLed + 1;
    return make_pair(firstLed, secondLed);
}

void lightUpAndLightDownGreenLed(pair<int, int> positions, De2iInterface interface) {
    cout << "Acendendo LED Verde na posição " << positions.first  << " e " << positions.second << endl;
    interface.writeGreenLed(1, positions.first);
    interface.writeGreenLed(1, positions.second);

    sleep(1);
    
    cout << "Apagando LED Verde na posição " << positions.first  << " e " << positions.second << endl;
    interface.writeGreenLed(0, positions.first);
    interface.writeGreenLed(0, positions.second);
    
    sleep(1);
}

void lightUpGreenLightFromVector(vector<int> array, De2iInterface interface) {
    for(unsigned int i = 0; i < array.size(); i++){
        pair<int, int> ledsToLightUp = translateButtonToGreenLed(array[i]);
        lightUpAndLightDownGreenLed(ledsToLightUp, interface);
    }
}

// popcnt == 1 é uma opção também
int isPowerOfTwo(unsigned n) {
    
    return n && (!(n & (n - 1)));
}

// Encontra a posição do único bit setado em 1; se houve mais de um bit em 1, retorna -1
// TODO: mudar isso para suportar instruções do x86 e quem sabe usar a diretiva de asm? 👀 
// tem uma lib pra isso em __builtin_clzll, acho que é a <limits>
int findPosition(unsigned n) {
    if (!isPowerOfTwo(n))
        return -1;
    
    unsigned i = 1, pos = 0;

    while (!(i & n)) {
        i = i << 1;
        ++pos;
    }
 
    return pos;
}

// --------------------------------------------------------------------------------
// Como são 8 leds e 4 botões, vamos considerar que as leds 0 e 1 são equivalentes ao botão 0, 
// a led 2 e 3 são equivalentes ao botão 2 e assim por diante
bool checkIfPositionOfButtonIsEquivalentOfGreenLight(pair<int, int> positionsOsLedsTranslated, int positionOfGreenLight) {
    cout << "Posição dos LEDs referentes ao botão clicado: " << positionsOsLedsTranslated.first << " " << positionsOsLedsTranslated.second << endl;
    cout << "Posição da LED piscado: " << positionOfGreenLight << endl;
    
    if(positionsOsLedsTranslated.first == positionOfGreenLight || positionsOsLedsTranslated.second == positionOfGreenLight)
        return true;
    
    return false;
}

void showPoints(De2iInterface interface) {
    interface.leftDisplayWrite(TOTAL_POINTS/10000);
    interface.rightDisplayWrite(TOTAL_POINTS%10000);
}

void updatePoints(De2iInterface interface) {
    TOTAL_POINTS++;
    showPoints(interface);
}

void resetPoints(De2iInterface interface) {
    TOTAL_POINTS = 0;
    showPoints(interface);
}

bool runRedLedsAndSwitchesAndCheckIfWin(De2iInterface interface) {
    return false;
}

GamePhase lostGame(De2iInterface interface) {
    // Se apertar um botão errado, acende todos os leds e retorna para a primeira fase
    interface.writeGreenLeds(0xffffffffu);
    interface.writeGreenLeds(0xffffffffu);
    
    sleep(1);
    cout << "Errou :( Voltando para o primeiro nível" << endl;
    return IntroPhase;
}
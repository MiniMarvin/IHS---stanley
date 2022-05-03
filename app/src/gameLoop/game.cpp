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
#include <random>

#include "../de2iInterface/de2iInterface.hpp"
#include "game.hpp"
#include "timer/timer.hpp"

using namespace std;

// ============================================================
// Types & Enums
// ============================================================
const int BUTTON_COUNT = 4;
int TOTAL_POINTS = 0;
int BUTTON_SEQUENCE_SIZE = 1;
int SWITCH_WORD_SIZE = 8;
int SWITCH_SECONDS_COUNT = 10;

// ============================================================
// Game Loop
// ============================================================
int gameLoop(char* driverPath) {
    cout << endl;
    cout << "Iniciando jogo..." <<endl;
    
    srand(time(0));
    
    De2iInterface interface = De2iInterface(driverPath);
    GamePhase phase = IntroPhase;
    
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
            cout << "IntroPhase" << endl;
            IntroPhaseImpl(interface);
            newPhase = ButtonPhase;
            break;
        }
        case ButtonPhase: {
            cout << "ButtonPhase" << endl;
            // TODO: fazer essas fases serem incrementais e o jogo ser no mínimo um jogo infinito
            // Primeira fase: acende aleatoriamente 5 valores de leds verde e compara com os botões apertados
            // TODO: add progress
            bool win = runGreenLedsAndPushButtonsGameAndCheckIfWin(BUTTON_SEQUENCE_SIZE, interface);
            if(win) newPhase = SwitchPhase;
            else newPhase = EndgamePhase;
            BUTTON_SEQUENCE_SIZE = BUTTON_SEQUENCE_SIZE + 1;
            
            interface.writeRedLeds(0);
            interface.writeGreenLeds(0);
            interface.rightDisplayWrite(0);
            interface.leftDisplayWrite(0);
            break;
        }
        case SwitchPhase: {
            cout << "SwitchPhase" << endl;
            int wordSize = SWITCH_WORD_SIZE, seconds = SWITCH_SECONDS_COUNT;
            newPhase = switchPhase(wordSize, seconds, interface);
            if (SWITCH_WORD_SIZE < 18) {
                SWITCH_WORD_SIZE += 1;
            } else {
                if (SWITCH_SECONDS_COUNT > 3) SWITCH_SECONDS_COUNT -= 1;
            }
            
            interface.writeRedLeds(0);
            interface.writeGreenLeds(0);
            interface.rightDisplayWrite(0);
            interface.leftDisplayWrite(0);
            break;
        }
        case EndgamePhase: {
            // TODO: ask for the user to insert it's username and maybe but just maybe add a scoreboard?
            newPhase = lostGame(interface);
            break;
        }
    }
    
    return newPhase;
}

void IntroPhaseImpl(De2iInterface interface) {
    for(int i = 17; i >= 0; i--) {
        interface.writeRedLed(1, i);
        usleep(100000);
        interface.writeRedLed(0, i);
        usleep(100000);
    }
    
    for(int i = 8; i >= 0; i--) {
        interface.writeGreenLed(1, i);
        usleep(100000);
        interface.writeGreenLed(0, i);
        usleep(100000);
    }
    
    interface.writeRedLeds(0xffffffffu);
    interface.writeGreenLeds(0xffffffffu);
    
    sleep(1);
    
    interface.writeRedLeds(0x000000000);
    interface.writeGreenLeds(0x000000000);
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
        if (buttons == prevValue || buttons == 15) {
            prevValue = buttons;
            continue;
        }
        
        cout << "Valor lido: " << buttons << endl;
        
        // Pega a posição do botão apertado; retorna -1 se nenhum ou mais de um forem apertados ao mesmo tempo.
        int positionOfButtonClicked = findPosition(buttons);
        cout << "Posição do botão clicado: " << positionOfButtonClicked << endl;
    
        pair<int, int> ledsToLightUp = translateButtonToGreenLed(positionOfButtonClicked);
        
        // Se a posição for igual a esperada
        if(checkIfPositionOfButtonIsEquivalentOfGreenLight(positionOfButtonClicked, orderToLightUp[LED_INDEX])) {
            lightUpAndLightDownGreenLed(ledsToLightUp, interface);
            
            // Se a posição for a última, vai para a próxima fase
            if(LED_INDEX == orderToLightUp.size() - 1) {
                cout << "Parabéns! Seguindo para o segundo nível!" << endl;
                return true;
            }
            
            LED_INDEX++;
            updatePoints(interface);
        } else {
            cout << "deu ruim" << endl;
            return false;
        }
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
}

void lightUpGreenLightFromVector(vector<int> array, De2iInterface interface) {
    for(unsigned int i = 0; i < array.size(); i++){
        pair<int, int> ledsToLightUp = translateButtonToGreenLed(array[i]);
        lightUpAndLightDownGreenLed(ledsToLightUp, interface);
        sleep(1);
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
    n = ~n;
    int idx = -1;
    for (int i = 0; i < 4; i++) {
        if (n & (1 << i)) {
            if (idx != -1) return -1;
            else idx = i;
        }
    }
    return idx;
}

// --------------------------------------------------------------------------------
// Como são 8 leds e 4 botões, vamos considerar que as leds 0 e 1 são equivalentes ao botão 0, 
// a led 2 e 3 são equivalentes ao botão 2 e assim por diante
bool checkIfPositionOfButtonIsEquivalentOfGreenLight(int buttonIndex, int positionOfGreenLight) {
    return buttonIndex == positionOfGreenLight;
}

void showPoints(De2iInterface interface) {
    // interface.leftDisplayWrite(TOTAL_POINTS/10000);
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
    usleep(500000);
    interface.writeGreenLeds(0x0);
    
    cout << "Errou :( Voltando para o primeiro nível" << endl;
    return IntroPhase;
}

unsigned int getMask(int wordSize) {
    unsigned int mask = 0u;
    for (int i = 0; i < wordSize; i++) {
        mask <<= 1;
        mask |= 1;
    }
    return mask;
}

unsigned int getRedLedsWord(int wordSize) {
    unsigned int mask = getMask(wordSize);
    return mask & random();
}

GamePhase switchPhase(int wordSize, int seconds, De2iInterface interface) {
    unsigned int word = getRedLedsWord(wordSize);
    unsigned int mask = getMask(wordSize);
    Timer timer = Timer();
    timer.init(seconds);
    GamePhase newPhase = EndgamePhase;
    cout << "binary word: " << (bitset<18>(word)) << endl;
    int prevTime = 0;
    
    while (1) {
        auto switches = interface.readSwitches() & mask;
        auto seconds = timer.missingSeconds();
        seconds *= 100;
        
        interface.leftDisplayWrite(seconds);
        interface.writeRedLeds(word);
        if (seconds != prevTime) {
            cout << "tick: " << seconds << "s" << endl;
            prevTime = seconds;
        }
        
        if (switches == word) {
            newPhase = ButtonPhase;
            updatePoints(interface);
            break;
        }
        if (timer.didFinish()) {
            break;
        }
    }
    
    return newPhase;
}
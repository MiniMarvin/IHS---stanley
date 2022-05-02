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

using namespace std;

// ============================================================
// Types & Enums
// ============================================================
#define USECONDS_60_FPS 16667

enum GamePhase {
    IntroPhase,
    ButtonPhase,
    SwitchPhase,
    EndgamePhase
};

typedef struct PerifericValues {
    unsigned int displayLeft;
    unsigned int displayRight;
    unsigned int redLeds;
    unsigned int greenLeds;
    PerifericValues(): displayLeft(0), displayRight(0), redLeds(0), greenLeds(0) {}
} PerifericValues;

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

int TOTAL_POINTS = 0;

// ============================================================
// Function Headers
// ============================================================
void printArray(vector<int> array);
vector<int> getOrderOfGreenLeds(int count);
pair<int, int> translateButtonToGreenLed(int position);
void lightUpAndLightDownGreenLed(pair<int, int> positions, De2iInterface interface);
void lightUpGreenLightFromVector(vector<int> array, De2iInterface interface);
int isPowerOfTwo(unsigned n);
int findPosition(unsigned n);
bool checkIfPositionOfButtonIsEquivalentOfGreenLight(pair<int, int> positionsOsLedsTranslated, int positionOfGreenLight);
void showPoints(De2iInterface interface);
void updatePoints(De2iInterface interface);
void resetPoints(De2iInterface interface);
bool runGreenLedsAndPushButtonsGameAndCheckIfWin(int numberOfLeds, De2iInterface interface);
bool runRedLedsAndSwitchesAndCheckIfWin(De2iInterface interface);
PerifericValues getPeriferics();
void configOmp(int threadCount);
std::chrono::time_point<std::chrono::high_resolution_clock> getCurrentTime();
long long getElapsedTime(TimePoint start);
void updatePeriferals(PerifericValues periferics, De2iInterface interface, TimePoint startTime);
GamePhase gameOperation(GamePhase phase, De2iInterface interface);


// ============================================================
// Game Loop
// ============================================================
int gameLoop(char* driverPath) {
    cout << endl;
    cout << "Iniciando jogo..." <<endl;
    
    De2iInterface interface = De2iInterface(driverPath);
    GamePhase phase = ButtonPhase;
    PerifericValues periferics;
    configOmp(2);
    
    while (1) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Here we define the OpenMP to split the operation of the game and the rendering of frames
        // Every function SHOULD NOT BE BLOCKING
        // Any activity that is time related may be updated wihtin another function that counts the values in a shared memory region
        #pragma omp parallel default(none) private(startTime) shared(periferics, interface, phase)
        {
            #pragma omp task
            updatePeriferals(periferics, interface, startTime);
            
            #pragma omp task
            phase = gameOperation(phase, interface);
        }
    }
    
	return 0;
}


// ============================================================
// Utility Functions
// ============================================================
void configOmp(int threadCount) {
    omp_set_dynamic(0);
    omp_set_num_threads(threadCount);
}

inline TimePoint getCurrentTime() {
    return std::chrono::high_resolution_clock::now();
}

long long getElapsedTime(TimePoint start) {
    auto elapsed = getCurrentTime() - start;
    unsigned long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    return microseconds;
}


// ============================================================
// Operational Fragments
// ============================================================
void updatePeriferals(PerifericValues periferics, De2iInterface interface, TimePoint startTime) {
    
    
    long long elapsedTime = getElapsedTime(startTime);
    long long missingTime = max(0ll, USECONDS_60_FPS - elapsedTime);
}


// TODO: dividir em duas threads, uma thread pra render da tela, idealmente um render burro 
// com uma região de memória compartilhada entre de onde vai ler o valor do display e o valor
// dos botões para poder renderizar tudo que tiver naquela região de memória, seria melhor
// ainda uma thread que nem faz isso mas é o que temos, daí nessa thread fazemos usleep(16667 - (end - init))
// para obter uma taxa de atualização de 60 quadros por segundo.
// TODO: dividir as fases de maneira incremental, o jogo vai ser basicamente dividido em algumas partes:
// 1. Inicio: Exibe uma tela de loading, pode ser qualquer coisa,como um shift de LED vermelho até a pessoa
// apertar start e dar inicio ao programa
// 2. Fase Botão: Vai Iniciar o Jogo exibindo para o usuário a sequência de botões para apertar
// 3. Fase Botão Entrada: Vai receber do usuário um input para poder comparar com o que o usuário digitou
// 4. Fase Switch: Vai mostrar ao usuário uma sequência de bits ON e OFF que o usuário precisa colocar e um timer
// daí o usuário precisa colocar essa fase antes do timer acabar
// 5. Fase de pontos: Após o usuário perder vai ser exibida a pontuação que ele fez e apertando start de novo
// O usuário volta para a fase 1
GamePhase gameOperation(GamePhase phase, De2iInterface interface) {
    GamePhase newPhase = phase;
    
    switch (phase) {
        case IntroPhase: {
            break;
        }
        case ButtonPhase: {
            // Primeira fase: acende aleatoriamente cinco valores de leds verde e compara com os botões apertados
            bool win = runGreenLedsAndPushButtonsGameAndCheckIfWin(5, interface);
            if(win) newPhase = SwitchPhase;
            break;
        }
        case SwitchPhase: {
            break;
        }
        case EndgamePhase: {
            break;
        }
    }
    
    return newPhase;
}


void printArray(vector<int> array) {
    cout << "Posições para ligar LEDs: ";
    
    for(unsigned int i = 0; i < array.size(); i++){
        cout << array[i] << " ";
    }
    
    cout << endl;
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
    interface.writeGreenLed(0, positions.first);
    interface.writeGreenLed(0, positions.second);

    sleep(1);
    
    cout << "Apagando LED Verde na posição " << positions.first  << " e " << positions.second << endl;
    interface.writeGreenLed(1, positions.first);
    interface.writeGreenLed(1, positions.second);
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

// ===============================================================================
// TODO: Uma opção melhor é a gente usar só 4 dos LEDs, é melhor pra usabilidade 
// e não confunde o jogador, não precisamos utilizar todos os periféricos, 
// é tranquilo pular alguns.
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

bool runGreenLedsAndPushButtonsGameAndCheckIfWin(int numberOfLeds, De2iInterface interface) {
    vector<int> orderToLightUp = getOrderOfGreenLeds(numberOfLeds);
    lightUpGreenLightFromVector(orderToLightUp, interface);
    
    cout << "Esperando botões..." << endl;
    unsigned int LED_INDEX = 0;
    // Loop para esperar interações do usuário
    while(1) {
        unsigned int buttons = interface.readButtons();
        cout << "Valor lido: " << buttons << endl;
        
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
            // Se apertar um botão errado, acende todos os leds e retorna para a primeira fase
            interface.writeGreenLeds(0b11111111);
            interface.writeGreenLeds(0b11111111);
            
            sleep(1);
            cout << "Errou :( Voltando para o primeiro nível" << endl;
            
            return false;
        }
        
        sleep(1);
    }
}

bool runRedLedsAndSwitchesAndCheckIfWin(De2iInterface interface) {
    return false;
}
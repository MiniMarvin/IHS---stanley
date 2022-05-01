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

#include "../de2iInterface/de2iInterface.hpp"

using namespace std;

int TOTAL_POINTS = 0;

void printArray(vector<int> array) {
    cout << "Posições para ligar LEDs: ";
    
    for(int i = 0; i < array.size(); i++){
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
    for(int i = 0; i < array.size(); i++){
        pair<int, int> ledsToLightUp = translateButtonToGreenLed(array[i]);
        lightUpAndLightDownGreenLed(ledsToLightUp, interface);
    }
}

int isPowerOfTwo(unsigned n) {
    return n && (!(n & (n - 1)));
}

// Encontra a posição do único bit setado em 1; se houve mais de um bit em 1, retorna -1
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
    int LED_INDEX = 0;
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
    
}

int gameLoop(char* driverPath) {
    cout << endl;
    cout << "Iniciando jogo..." <<endl;
    
    De2iInterface interface = De2iInterface(driverPath);
    
    // Primeira fase: acende aleatoriamente cinco valores de leds verde e compara com os botões apertados
    first_level:
        resetPoints(interface);
        bool win = runGreenLedsAndPushButtonsGameAndCheckIfWin(5, interface);
       
        if(win) goto second_level;
        goto first_level;
        
    // Segunda fase: 
    second_level:

    
	return 0;
}

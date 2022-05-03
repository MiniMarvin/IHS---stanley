#ifndef __PERIFERIC_VALUES
#define __PERIFERIC_VALUES

#include <omp.h>
#include <iostream>

using namespace std;

class PerifericValues {
private:
    unsigned int displayLeft;
    unsigned int displayRight;
    unsigned int redLeds;
    unsigned int greenLeds;
public:
    PerifericValues(): displayLeft(0), displayRight(0), redLeds(0), greenLeds(0) {}
    
    void setDisplayLeft(unsigned int value) {
        #pragma omp atomic write
        this->displayLeft = value;
    }
    
    void setDisplayRight(unsigned int value) {
        #pragma omp atomic write
        this->displayRight = value;
    }
    
    void setRedLeds(unsigned int value) {
        #pragma omp atomic write
        this->redLeds = value;
    }
    
    void setGreenLeds(unsigned int value) {
        #pragma omp atomic write
        this->greenLeds = value;
    }
    
    void setRedLed(bool bit, int index) {
        cout << "Update bit to " << bit << " at index " << index << endl;
        unsigned int mask;
        
        if (bit) {
            mask = ~(bit << index);
            #pragma omp atomic
            redLeds = redLeds & mask;    
        } else {
            mask = bit << index;
            #pragma omp atomic
            redLeds = redLeds | mask;    
        }
        
        cout << "mask: " << mask << endl;
        cout << "greenLeds: " << redLeds << endl;
    }
    
    void setGreenLed(bool bit, int index) {
        cout << "Update bit to " << bit << " at index " << index << endl;
        unsigned int mask;
        
        if (bit) {
            mask = ~(bit << index);
            #pragma omp atomic
            greenLeds = greenLeds & mask;    
        } else {
            mask = bit << index;
            #pragma omp atomic
            greenLeds = greenLeds | mask;    
        }
        
        cout << "mask: " << mask << endl;
        cout << "greenLeds: " << greenLeds << endl;
    }
    
    unsigned int getDisplayLeft() {
        unsigned int data;
        #pragma omp atomic read
        data = this->displayLeft;
        return data;
    }
    
    unsigned int getDisplayRight() {
        unsigned int data;
        #pragma omp atomic read
        data = this->displayRight;
        return data;
    }
    
    unsigned int getRedLeds() {
        unsigned int data;
        #pragma omp atomic read
        data = this->redLeds;
        return data;
    }
    
    unsigned int getGreenLeds() {
        unsigned int data;
        #pragma omp atomic read
        data = this->greenLeds;
        return data;
    }
};
#endif
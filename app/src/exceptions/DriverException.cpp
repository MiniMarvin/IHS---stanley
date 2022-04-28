#include "DriverException.hpp"
#include <iostream>

using namespace std;

const char* DriverException::what() const throw () {
    return this->msg.c_str();
}

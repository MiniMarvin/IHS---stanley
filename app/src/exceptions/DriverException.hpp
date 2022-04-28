#ifndef __DRIVER_EXCEPTION
#define __DRIVER_EXCEPTION

#include <string>
using namespace std;

class DriverException : public exception {
private:
    const string& msg;
public:
    DriverException(const string& msg) : msg(msg) {}
    virtual const char* what() const throw ();
};


#endif
#ifndef __D2I_INTERFACE
#define __D2I_INTERFACE

class D2iInterface {
private:
    int fileDescriptor;
public:
    D2iInterface(char* driverPath);
    ~D2iInterface();
};

#endif

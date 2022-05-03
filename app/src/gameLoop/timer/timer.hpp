#ifndef __MY_TIMER
#define __MY_TIMER
#include <chrono>

#define USECONDS_60_FPS 16667

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;

class Timer {
private:
    TimePoint start;
    unsigned int totalMicroSeconds;
public:
    Timer() {}
    void init(int seconds);
    void uinit(unsigned long long useconds);
    long long missingUSeconds();
    int missingSeconds();
    bool didFinish();
};

inline TimePoint getCurrentTime();
long long getElapsedTime(TimePoint start);

#endif

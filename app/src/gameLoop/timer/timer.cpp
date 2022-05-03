#include "timer.hpp"
#include <algorithm>
#include <chrono>

using namespace std;

inline TimePoint getCurrentTime() {
    return std::chrono::high_resolution_clock::now();
}

long long getElapsedTime(TimePoint start) {
    auto elapsed = getCurrentTime() - start;
    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    return microseconds;
}

// Timer
void Timer::init(int seconds) {
    this->uinit(1000000ull * (unsigned long long) seconds);
}

void Timer::uinit(unsigned long long useconds) {
    auto startTime = getCurrentTime();
    this->start = startTime;
    this->totalMicroSeconds = useconds;
}

long long Timer::missingUSeconds() {
    long long elapsedTime = getElapsedTime(this->start);
    return max(0ll, this->totalMicroSeconds - elapsedTime);
}

int Timer::missingSeconds() {
    long long useconds = this->missingUSeconds();
    return (int) (useconds/1000000ll);
}

bool Timer::didFinish() {
    return this->missingUSeconds() == 0;
}


#include "timer.hpp"
#include <algorithm>
#include <chrono>
#include <omp.h>

using namespace std;

inline TimePoint getCurrentTime() {
    return std::chrono::high_resolution_clock::now();
}

long long getElapsedTimeBetween(TimePoint start, TimePoint end) {
    auto elapsed = end - start;
    long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    return microseconds;
}

long long getElapsedTime(TimePoint start) {
    return getElapsedTimeBetween(start, getCurrentTime());
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
    if (this->isLocked) return this->totalMicroSeconds;
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

void Timer::lock() {
    #pragma omp critical (time_lock)
    {
        this->isLocked = true;
        auto current = getCurrentTime();
        auto elapsedTime = getElapsedTimeBetween(this->start, current);
        auto miss = max(0ll, this->totalMicroSeconds - elapsedTime);
        this->totalMicroSeconds = miss;
    }
}

void Timer::unlock() {
    #pragma omp critical (time_lock)
    {
        this->start = getCurrentTime();
        this->isLocked = false;
    }
}
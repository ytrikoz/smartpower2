#pragma once

#include <Arduino.h>

#include "Strings.h"

#define LOOP_COUNTERS 10
#define MODULES_COUNT 13
#define LOOP_CAPTURE_INTERVAL 10000

namespace Profiler {

enum Module {BUTTONS, CLOCK, LEDS, PSU, TASKS, CONSOLE, LCD, HTTP, DISCOVERY, NTP, TELNET, SHELL, OTA_UPDATE};

enum State { CAPTURE_IDLE, CAPTURE_IN_PROGRESS, CAPTURE_DONE };

class LoopWatchDog {
   public:
    LoopWatchDog();
    void run();
    void resetStats();
    void startCapture();
    void printCapture(Print* p);
    State getState();
    void setIdle();
    unsigned long getCaptureTimeLeft();
    void addTimeProfile(uint8_t index, unsigned long duration);
   private:
    State state;
    unsigned long captureTimeLeft;
    unsigned long captureStarted;
    unsigned long captureFinished;
    unsigned long loopStarted;

    uint32_t loopCounter = 0;
    uint32_t loops[LOOP_COUNTERS];
    #ifdef DEBUG_LOOP
    unsigned long modules[MODULES_COUNT];
    #endif
    uint32_t loopOverRange = 0;
    unsigned long loopLongest = 0;    
};


class TimeProfiler {
   public:
    TimeProfiler(LoopWatchDog *watchDog, Module index);
    ~TimeProfiler();
    unsigned long duration();
   private:
    Profiler::Module module;
    LoopWatchDog *watchDog;
    unsigned long started;
    unsigned long finished;
};

inline TimeProfiler::TimeProfiler(LoopWatchDog* watchDog, Module module) {
    this->watchDog = watchDog;
    this->module = module;
    started = micros();
}

inline unsigned long TimeProfiler::duration(){
    return finished - started;
}

inline TimeProfiler::~TimeProfiler() {
    finished =  micros();
    watchDog->addTimeProfile(module, finished - started);
}

}  // namespace Profiler
#pragma once

#include <Arduino.h>

#include "Consts.h"

#define LOOP_COUNTERS 9
#define LOOP_CAPTURE_INTERVAL 10000

namespace Profiler {

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
   private:
    State state;
    long interval = 0;
    unsigned long captureTimeLeft;
    unsigned long captureStarted;
    unsigned long captureFinished;
    unsigned long loopStarted;

    uint32_t loopCounter = 0;
    uint32_t loops[LOOP_COUNTERS];
    uint32_t loopOverRange = 0;
    unsigned long loopLongest = 0;
};

}  // namespace Profiler
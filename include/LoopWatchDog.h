#pragma once

#include <Arduino.h>

#include "Consts.h"

typedef enum { CAPTURE_IDLE, CAPTURE_IN_PROGRESS, CAPTURE_DONE } WatchDogState;

class LoopWatchDog {
   private:
    WatchDogState state = CAPTURE_IDLE;
    long interval = 0;
    unsigned long captureTimeLeft = 0;
    unsigned long captureStart = 0, captureFinish = 0;
    unsigned long loopStarted = 0;
    long elapsed = 0;

    unsigned long loops = 0;
    unsigned long loops2ms = 0;
    unsigned long loops4ms = 0;
    unsigned long loops8ms = 0;
    unsigned long loops16ms = 0;
    unsigned long longWaitLoops = 0;
    unsigned long longestWait = 0;

   public:
    LoopWatchDog(/* args */);
    ~LoopWatchDog();
    void run();
    void resetStats();
    void startCapture();
    WatchDogState getState();
    void printDiag(Print* p);
    void _printDiag(Print* p);
};

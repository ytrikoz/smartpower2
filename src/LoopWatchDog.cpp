#include "LoopWatchDog.h"

#include "consts.h"
#include "time_utils.h"

LoopWatchDog::LoopWatchDog() {
    resetStats();
    captureTimeLeft = 0;
    captureStart = millis();
    captureFinish = 0;
    loopStarted = 0;
}

LoopWatchDog::~LoopWatchDog() {}

void LoopWatchDog::_printDiag(Print* p) {
    long elapsed = millis_passed(captureStart, captureFinish);
    unsigned long lps = floor(((float)loops / elapsed) * ONE_SECOND_ms);

    p->print(FPSTR(str_elapsed));
    p->printf_P(strf_lu_ms, elapsed);
    p->print(F("total "));
    p->print(loops);
    p->print((" lps "));
    p->println(lps);
    p->print(" 2ms ");
    p->print(loops2ms);
    p->print(" 4ms ");
    p->print(loops4ms);
    p->print(" 8ms ");
    p->print(loops8ms);
    p->print(" 16ms ");
    p->println(loops16ms);

    if (longWaitLoops > 0) {
        p->print(" over 16ms ");
        p->print(longWaitLoops);
        p->print(" longest ");
        p->printf_P(strf_lu_ms, longestWait);
        p->println();
    }
}

void LoopWatchDog::printDiag(Print* p) {
    if (getState() == CAPTURE_IN_PROGRESS) {
        p->print("capturing ");
        p->printf_P(strf_lu_ms, captureTimeLeft);
        p->println();
        return;
    };
 
    if (getState() == CAPTURE_IDLE) {
        startCapture();    
        p->print("capture for ");
        p->printf_P(strf_lu_ms, captureTimeLeft);
        p->println();
        return;
    }

    if (getState() == CAPTURE_DONE)    
    {
        _printDiag(p);
        state = CAPTURE_IDLE;
    };
}

WatchDogState LoopWatchDog::getState() 
{
    return state;
}

void LoopWatchDog::startCapture()
{
    resetStats();

    captureStart = millis();
    captureFinish = 0;
    captureTimeLeft = 10000;

    loopStarted = 0;    

    state = CAPTURE_IN_PROGRESS;    
}

void LoopWatchDog::run() {
    if (getState() != CAPTURE_IN_PROGRESS) return;

    if (loopStarted == 0) {        
        loopStarted = millis();
        return;
    }
    long elapsed = millis_since(loopStarted);
    loops++;
    if (elapsed < 2) {
        loops2ms++;
    } else if ((elapsed >= 2) && (elapsed < 4)) {
        loops4ms++;
    } else if ((elapsed >= 4) && (elapsed < 8)) {
        loops8ms++;
    } else if ((elapsed >= 8) && (elapsed < 16)) {
        loops16ms++;
    } else
        longWaitLoops++;

    if (longestWait < (unsigned long) elapsed) longestWait = elapsed;

    if (captureTimeLeft <= (unsigned long) elapsed) {
        captureTimeLeft = 0;       
        captureFinish = millis();        
        state = CAPTURE_DONE;
        return; 
    }
    captureTimeLeft -= elapsed;
    loopStarted = millis();
}

void LoopWatchDog::resetStats() {
    loops = 0;
    loops2ms = 0;
    loops4ms = 0;
    loops8ms = 0;
    loops16ms = 0;
    longWaitLoops = 0;
    longestWait = 0;
}
#include "LoopWatchDog.h"

#include "Strings.h"
#include "TimeUtils.h"

namespace Profiler {

LoopWatchDog::LoopWatchDog() {
    captureTimeLeft = 0;
    loopStarted = 0;
    state = CAPTURE_IDLE;
}

unsigned long LoopWatchDog::getCaptureTimeLeft() { return captureTimeLeft; }

void LoopWatchDog::printCapture(Print* p) {
    p->print(FPSTR(str_capture));
    long duration = millis_passed(captureStarted, captureFinished);
    p->printf_P(strf_lu_ms, duration);
    p->println();

    p->print(FPSTR(str_total));
    p->print(loopCounter);
    uint16_t lps = floor((float)loopCounter / duration * ONE_SECOND_ms);
    p->print((" lps "));
    p->println(lps);

    unsigned long time = 2;
    for (uint8_t i = 0; i < LOOP_COUNTERS; ++i) {
        if (loops[i] > 0) {
            p->printf_P(strf_lu_ms, time);
            p->println(loops[i]);
        }
        time *= 2;
    }

    if (loopOverRange > 0) {
        p->print(FPSTR(str_over));
        p->printf_P(strf_lu_ms, time / 2);
        p->print(loopOverRange);
    }

    p->print(FPSTR(str_max));
    p->printf_P(strf_lu_ms, (unsigned long)loopLongest);
    p->println();
}

void LoopWatchDog::setIdle() { state = CAPTURE_IDLE; }

State LoopWatchDog::getState() { return state; }

void LoopWatchDog::startCapture() {
    resetStats();

    captureStarted = 0;
    captureFinished = 0;
    captureTimeLeft = LOOP_CAPTURE_INTERVAL;

    loopStarted = 0;

    state = CAPTURE_IN_PROGRESS;
}

void LoopWatchDog::run() {
    if (getState() != CAPTURE_IN_PROGRESS) return;

    if (loopStarted == 0) {
        loopStarted = millis();
        captureStarted = loopStarted;
        return;
    }

    unsigned long passed = millis_since(loopStarted);

    loopCounter++;

    unsigned long time = 2;
    for (uint8_t i = 0; i < LOOP_COUNTERS; ++i) {
        if (passed <= time) {
            loops[i]++;
            break;
        }
        time *= 2;
    }
    if (loopLongest < passed) loopLongest = passed;

    if (captureTimeLeft <= passed) {
        captureTimeLeft = 0;
        captureFinished = millis();
        state = CAPTURE_DONE;
        return;
    }
    captureTimeLeft -= passed;

    loopStarted = millis();
}

void LoopWatchDog::resetStats() {
    loopCounter = 0;
    memset(loops, 0, sizeof(loops[0]) * LOOP_COUNTERS);
    loopOverRange = 0;
    loopLongest = 0;
}

}  // namespace Profiler

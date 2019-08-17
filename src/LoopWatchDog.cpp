#include "LoopWatchDog.h"

#include "Strings.h"
#include "TimeUtils.h"

namespace Profiler {

LoopWatchDog::LoopWatchDog() {
    captureTimeLeft = 0;
    loopStarted = 0;
    state = CAPTURE_IDLE;
}

#ifdef DEBUG_LOOP
void LoopWatchDog::addTimeProfile(uint8_t index, unsigned long duration) {
    if (state == CAPTURE_IN_PROGRESS) modules[index] += duration;
}
#endif

unsigned long LoopWatchDog::getCaptureTimeLeft() { return captureTimeLeft; }

void LoopWatchDog::printCapture(Print* p) {
    p->print(FPSTR(str_capture));
    long duration = millis_passed(captureStarted, captureFinished);
    p->printf_P(strf_lu_ms, duration);
    p->println();

    p->print(FPSTR(str_total));
    p->print(loopCounter);
    uint16_t lps = floor((float)loopCounter / duration * ONE_SECOND_ms);
    p->print(F(" lps "));
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
        p->println(loopOverRange);
    }

    p->print(FPSTR(str_max));
    p->printf_P(strf_lu_ms, loopLongest);
    p->println();

#ifdef DEBUG_LOOP
    float total_load = 0;
    for (uint8_t i = 0; i < MODULES_COUNT; ++i) {
        p->print(getModuleName(i));
        float load = (float) (modules[i] / ONE_MILLISECOND_mi) / duration * 100;
        p->printf_P(strf_per, load);
        total_load += load;
        p->println();
    }
    p->print(FPSTR(str_system_time));
    p->printf_P(strf_per, 100 - total_load);
    p->println();
#endif
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
    uint8_t i;
    for (i = 0; i < LOOP_COUNTERS; ++i) {
        if (passed <= time) {
            loops[i]++;
            break;
        }
        time *= 2;
    }
    if (loopLongest < passed) loopLongest = passed;
    if (i == LOOP_COUNTERS) loopOverRange++;

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
    #ifdef DEBUG_LOOP
    memset(modules, 0, sizeof(modules[0]) * MODULES_COUNT);
    #endif
    loopOverRange = 0;
    loopLongest = 0;
}

}  // namespace Profiler

#include "LoopWatchDog.h"

#include "TimeUtils.h"

namespace Profiler {

LoopWatchDog::LoopWatchDog() { state = CAPTURE_IDLE; }

void LoopWatchDog::setIdle() { state = CAPTURE_IDLE; }

State LoopWatchDog::getState() { return state; }

Capture *LoopWatchDog::getResults() { return &cap; }

void LoopWatchDog::add(Module module, unsigned long duration) {
    if (state == CAPTURE_IN_PROGRESS) cap.module[module] += duration;
}

unsigned long LoopWatchDog::getDuration() {
    switch (state) {
        case CAPTURE_IN_PROGRESS:
            return captureTimeleft;
        case CAPTURE_IDLE:
            return LOOP_CAPTURE_INTERVAL;
        case CAPTURE_DONE:
            return cap.duration;
        default:
            return 0;
    }
}

void LoopWatchDog::start() {
    cap.reset();
    captureTimeleft = LOOP_CAPTURE_INTERVAL;
    state = CAPTURE_IN_PROGRESS;
    loopStarted = 0;
}

void LoopWatchDog::loop() {
    if (state != CAPTURE_IN_PROGRESS) return;

    if (loopStarted == 0) {
        loopStarted = millis();
        return;
    }
    unsigned long now = millis();
    unsigned long passed = millis_passed(loopStarted, now);
    
    cap.total++;
    unsigned long time_range = 2;
    uint8_t i;
    for (i = 0; i < LOOP_COUNTERS; ++i) {
        if (passed <= time_range) {
            cap.counter[i]++;
            break;
        }
        time_range *= 2;
    }
    if (cap.longest < passed) cap.longest = passed;
    if (i == LOOP_COUNTERS) cap.overrange++;

    cap.duration += passed;    
    if (captureTimeleft <= passed) {
        captureTimeleft = 0;
        state = CAPTURE_DONE;                
        return;
    }
    captureTimeleft -= passed;
    loopStarted = millis();
}

TimeProfiler LoopWatchDog::run(Module module) {
    return TimeProfiler(this, module);
}

TimeProfiler::TimeProfiler(ProfilesHolder* holder, Module module) {
    this->holder = holder;
    this->module = module;
    started = micros();
}

TimeProfiler::~TimeProfiler() {
    this->holder->add(this->module, micros() - this->started);
}

}  // namespace Profiler

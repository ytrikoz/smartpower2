#include "LoopWatcher.h"

LoopWatcher::LoopWatcher() {
    state = CAPTURE_IDLE;
    captureStarted = 0;
}

void LoopWatcher::setIdle() { state = CAPTURE_IDLE; }

LoopWatcherState LoopWatcher::getState() { return state; }

void LoopWatcher::logTime(AppModuleEnum module, unsigned long duration) {
    if (state == CAPTURE_IN_PROGRESS)
        cap.module[module] += duration;
}

LoopCapture *LoopWatcher::getCapture() { return &cap; }

unsigned long LoopWatcher::getDuration() {
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

void LoopWatcher::start() {
    cap.reset();
    captureTimeleft = LOOP_CAPTURE_INTERVAL;
    state = CAPTURE_IN_PROGRESS;
    loopStarted = 0;
}

void LoopWatcher::loop() {
    if (state != CAPTURE_IN_PROGRESS)
        return;

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
    if (cap.longest < passed)
        cap.longest = passed;
    if (i == LOOP_COUNTERS)
        cap.overrange++;

    cap.duration += passed;
    if (captureTimeleft <= passed) {
        captureTimeleft = 0;
        state = CAPTURE_DONE;
        return;
    }
    captureTimeleft -= passed;
    loopStarted = millis();
}

LiveTimer LoopWatcher::onExecute(AppModuleEnum module) {
    return LiveTimer(this, module);
}

LiveTimer::LiveTimer(LiveTimerLogger *logger, AppModuleEnum module) {
    this->logger = logger;
    this->module = module;
    createTime = micros();
}

LiveTimer::~LiveTimer() { logger->logTime(module, micros() - createTime); }

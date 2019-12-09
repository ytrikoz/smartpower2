#pragma once

#include <Arduino.h>

#include "Strings.h"
#include "TimeUtils.h"

#define LOOP_COUNTERS 10
#define LOOP_CAPTURE_INTERVAL 10000

struct LoopCapture {
    unsigned long duration;
    uint32_t total;
    uint32_t overrange;
    unsigned long longest;
    uint32_t counter[LOOP_COUNTERS];
    uint8_t counters_size;
    unsigned long module[APP_MODULES];
    uint8_t modules_size;
    LoopCapture() { reset(); }
    void reset() {
        duration = 0;
        total = 0;
        overrange = 0;
        longest = 0;
        counters_size = LOOP_COUNTERS;
        memset(counter, 0, sizeof(counter[0]) * counters_size);
        modules_size = APP_MODULES;
        memset(module, 0, sizeof(module[0]) * modules_size);
    }
};

enum LoopWatcherState { CAPTURE_IDLE, CAPTURE_IN_PROGRESS, CAPTURE_DONE };

class LiveTimerLogger {
   public:
    virtual void logTime(AppModuleEnum, unsigned long);
};

class LiveTimer {
   public:
    LiveTimer(LiveTimerLogger*, AppModuleEnum);
    ~LiveTimer();

   private:
    unsigned long createTime;
    AppModuleEnum module;
    LiveTimerLogger* logger;
};

class LoopWatcher : public LiveTimerLogger {
   public:
    void logTime(AppModuleEnum, unsigned long);

   public:
    LoopWatcher();
    void loop();
    void start();

    LoopWatcherState getState();
    unsigned long getDuration();
    LoopCapture* getCapture();
    void setIdle();
    LiveTimer onExecute(AppModuleEnum);

    LoopWatcherState state;

   private:
    LoopCapture cap;
    unsigned long captureStarted;
    unsigned long captureTimeleft;
    unsigned long loopStarted;
};

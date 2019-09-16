#pragma once

#include <Arduino.h>

#include "Strings.h"

#define MODULES_COUNT 13
#define LOOP_COUNTERS 10
#define LOOP_CAPTURE_INTERVAL 10000

namespace Profiler {

struct Capture {
    unsigned long duration;
    uint32_t total;
    uint32_t overrange;
    unsigned long longest;
    uint32_t counter[LOOP_COUNTERS];
    uint8_t counters_size;
    unsigned long module[MODULES_COUNT];
    uint8_t modules_size;
    Capture() {
        reset();
    }
    void reset() 
    {
        duration = 0;
        total = 0;
        overrange = 0;
        longest = 0;
        counters_size = LOOP_COUNTERS;
        memset(counter, 0, sizeof(counter[0]) * counters_size);
        modules_size = MODULES_COUNT;
        memset(module, 0, sizeof(module[0]) * modules_size);

    }
};

enum Module {
    BUTTONS,
    CLOCK,
    LEDS,
    PSU,
    TASKS,
    SERIAL_SHELL,
    LCD,
    HTTP,
    NETSVC,
    NTP,
    TELNET,
    TELNET_SHELL,
    OTA_UPDATE
};

enum State { CAPTURE_IDLE, CAPTURE_IN_PROGRESS, CAPTURE_DONE };

class ProfilesHolder {
   public:
    virtual void add(Module module, unsigned long duration);
};

class TimeProfiler {
   public:
    TimeProfiler(ProfilesHolder* holder, Module module);
    ~TimeProfiler();

   private:
    Module module;
    ProfilesHolder* holder;
    unsigned long started;
};

class LoopWatchDog : public ProfilesHolder {
   public:
    LoopWatchDog();
    void loop();
    void start();
    void add(Module module, unsigned long duration);
    Capture* getResults();
    State getState();
    unsigned long getDuration();
    void setIdle();
    TimeProfiler run(Module module);

   private:
    Capture cap;
    State state;
    unsigned long captureStarted;
    unsigned long captureTimeleft;
    unsigned long loopStarted;
};

}  // namespace Profiler
#pragma once

#include <Arduino.h>

#include "Strings.h"
#include "Utils/TimeUtils.h"

#define LOOP_COUNTERS 8
#define LOOP_CAPTURE_INTERVAL 60000

enum LoopTimerState { CAPTURE_IDLE,
                      CAPTURE_PROGRESS,
                      CAPTURE_DONE };

struct LoopCapture {
    unsigned long duration;
    uint32_t total;
    uint32_t max_time_counter;
    unsigned long max_time;
    uint32_t counter[LOOP_COUNTERS];
    uint8_t counters_size;
    unsigned long module[APP_MODULES];
    uint8_t modules_size;

    LoopCapture() { reset(); }

    void reset() {
        duration = 0;
        total = 0;
        max_time_counter = 0;
        max_time = 0;
        counters_size = LOOP_COUNTERS;
        modules_size = APP_MODULES;
        memset(counter, 0, sizeof(counter[0]) * LOOP_COUNTERS);
        memset(module, 0, sizeof(module[0]) * APP_MODULES);
    }
};

class RunTimerHost {
   public:
    virtual void onRunTimer(ModuleEnum, unsigned long);
};

class RunTimer {
   public:
    RunTimer(RunTimerHost* host, ModuleEnum module) : host_(host), module_(module), start_(micros()) {}

    ~RunTimer() { host_->onRunTimer(module_, micros() - start_); }

   private:
    RunTimerHost* host_;
    ModuleEnum module_;
    unsigned long start_;
};

class LoopTimer : public RunTimerHost {
   public:
    void onRunTimer(ModuleEnum item, unsigned long time) {
        if (state_ == CAPTURE_PROGRESS)
            data_.module[item] += time;
    }

   public:
    LoopTimer() : state_(CAPTURE_IDLE), start_(0) {}

    void loop() {
        if (state_ != CAPTURE_PROGRESS) return;
       
        if (!start_) {
            start_ = millis();
            return;
        }

        unsigned long now = millis();
        unsigned long passed = millis_passed(start_, now);

        data_.total++;
        unsigned long range = 2;
        uint8_t i;
        for (i = 0; i < LOOP_COUNTERS; ++i) {
            if (passed <= range) {
                data_.counter[i]++;
                break;
            }
            range *= 2;
        }

        if (data_.max_time < passed) data_.max_time = passed;

        if (i == LOOP_COUNTERS) data_.max_time_counter++;

        data_.duration += passed;

        if (left_ <= passed) {
            left_ = 0;
            state_ = CAPTURE_DONE;
            return;
        }
        left_ -= passed;
        start_ = millis();
    }

    void start() {
        data_.reset();
        left_ = LOOP_CAPTURE_INTERVAL;
        state_ = CAPTURE_PROGRESS;
    }

    void idle() { state_ = CAPTURE_IDLE; }

    LoopTimerState getState() { return state_; }

    LoopCapture* getData() { return &data_; }

    RunTimer start(uint8_t index) {
        return start(ModuleEnum(index));
    }

    RunTimer start(ModuleEnum module) {
        return RunTimer(this, module);
    }

   private:
    LoopTimerState state_;
    LoopCapture data_;
    unsigned long left_;
    unsigned long start_;
};

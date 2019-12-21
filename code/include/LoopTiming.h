#pragma once

#include <Arduino.h>

#include "Strings.h"
#include "Utils/TimeUtils.h"

#define TIME_RANGES 8
#define LOOP_CAPTURE_INTERVAL 10000

enum LoopTimerState { CAPTURE_IDLE,
                      CAPTURE_PROGRESS,
                      CAPTURE_DONE };

struct LoopCapture : Printable {
    unsigned long loop_;
    unsigned long cnt_;
    unsigned long overrange_cnt_;
    unsigned long max_;
    uint8_t time_size_;
    uint8_t module_size_;
    unsigned long time_[TIME_RANGES];
    unsigned long module_[APP_MODULES];

    LoopCapture() : loop_(0),
                    cnt_(0),
                    overrange_cnt_(0),
                    max_(0),
                    time_size_(TIME_RANGES),
                    module_size_(APP_MODULES),
                    time_{0},
                    module_{0} {}

    void module(uint8_t index, const unsigned long time) {
        module_[index] += time;
    }

    void loop(const unsigned long time) {
        cnt_++;
        unsigned long range = 2;
        uint8_t i;
        for (i = 0; i <= time_size_ - 1; ++i) {
            if (time <= range) {
                time_[i]++;
                break;
            }
            range *= 2;
        }
        if (time > range) {
            overrange_cnt_++;
        }
        if (max_ < time) {
            max_ = time;
        }
        loop_ += time;
    }

    size_t printTo(Print& p) const {
        size_t n = 0;
        unsigned long range = 2;
        uint8_t i;
        for (i = 0; i <= time_size_ - 1; ++i) {
            if (time_[i]) {
                n += p.print(range < max_ ? range : max_);
                n += p.print('\t');
                n += p.println(time_[i]);
            }
            range *= 2;
        }
        if (overrange_cnt_) {
            n += p.print(FPSTR(str_over));
            n += p.print('\t');
            n += p.print(overrange_cnt_);
            n += p.println(max_);
        }
        n += p.print(FPSTR(str_total));
        n += p.print('\t');
        n += p.print(cnt_);
        n += p.print('\t');
        n += p.println((float)loop_ / cnt_);

        unsigned long modules_time = 0;
        for (uint8_t i = 0; i < module_size_ - 1; ++i) {
            modules_time += module_[i];
            n += p.print(i);
        }
        n += p.println();
        for (uint8_t i = 0; i < module_size_ - 1; ++i) {
            float per = (float) module_[i] / modules_time * 100;
            n += p.print(per);
            n += p.print('\t');
        }
        unsigned long other_time = loop_ - modules_time / 1000;
        n += p.print(FPSTR(str_other));
        n += p.print('\t');
        n += p.print( (float) other_time / loop_ * 100);
        n += p.println();
        return n;
    }
};

class LoopTimer {
   public:
    void onResult(ModuleEnum item, unsigned long time) {
        if (state_ == CAPTURE_PROGRESS)
            data_.module(item, time);
    }

   public:
    LoopTimer() : state_(CAPTURE_IDLE), start_(0) {}

    void tick() {
        if (state_ != CAPTURE_PROGRESS) return;

        if (!start_) {
            start_ = millis();
            return;
        }
        unsigned long now = millis();
        unsigned long passed = millis_passed(start_, now);

        data_.loop(passed);

        if (left_ <= passed) {
            left_ = 0;
            state_ = CAPTURE_DONE;
            return;
        }
        left_ -= passed;
        start_ = millis();
    }

    void start() {
        data_ = LoopCapture();
        left_ = LOOP_CAPTURE_INTERVAL;
        state_ = CAPTURE_PROGRESS;
    }

    void idle() { state_ = CAPTURE_IDLE; }

    LoopTimerState getState() { return state_; }

    LoopCapture getData() { return data_; }

    void start(uint8_t index) {
        mod_ = ModuleEnum(index);
        mod_start_ = micros();
    }

    void end() {
        onResult(mod_, micros() - mod_start_);
    }

   private:
    LoopTimerState state_;
    LoopCapture data_;
    unsigned long left_;
    unsigned long start_;
    unsigned long mod_start_;
    ModuleEnum mod_;
};

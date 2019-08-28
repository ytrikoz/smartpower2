#pragma once

#include "Actions.h"


namespace Actions {

class ShowLoop : public Action {
   public:
    void exec(Print* p);
    void printCapture(Print* p, Profiler::Capture* cap);
};

void ShowLoop::exec(Print* p) {
    Profiler::State state = watchDog.getState();
    switch (state) {
        case Profiler::CAPTURE_IDLE:
            p->print(getStrP(str_start));
            p->print(getStrP(str_capture));
            p->printf_P(strf_lu_ms, watchDog.getDuration());
            p->println();
            watchDog.start();
            break;
        case Profiler::CAPTURE_IN_PROGRESS:
            p->print(getStrP(str_capturing));
            p->printf_P(strf_lu_ms, watchDog.getDuration());
            p->println();
            break;
        case Profiler::CAPTURE_DONE:
            this->printCapture(p, watchDog.getResults());
            watchDog.setIdle();
    };
}

void ShowLoop::printCapture(Print* p, Profiler::Capture* cap) {
    p->print(getStrP(str_capture));
    p->printf_P(strf_lu_ms, cap->duration);
    p->println();

    unsigned long time_range = 2;
    for (uint8_t i = 0; i < cap->counters_size; ++i) {
        if (cap->counter[i] > 0) {
            if (time_range > cap->longest)
                p->printf_P(strf_lu_ms, cap->longest);
            else
                p->printf_P(strf_lu_ms, time_range);
            p->print('\t');
            p->println(cap->counter[i]);
        }
        time_range *= 2;
    }

    if (cap->overrange > 0) {
        p->print(getStrP(str_over));
        p->printf_P(strf_lu_ms, time_range / 2);
        p->print('\t');
        p->println(cap->overrange);
        p->print('\t');
        p->printf_P(strf_lu_ms, cap->longest);
        p->println();
    }

    p->print(getStrP(str_total));
    p->print('\t');
    p->print(cap->total);
    p->print('\t');
    p->println((float)cap->duration / cap->total);

    float total_modules_time = 0;
    for (uint8_t i = 0; i < cap->modules_size; ++i)
        total_modules_time += floor((float)cap->module[i] / ONE_MILLISECOND_mi);
    
    float system_time = cap->duration - total_modules_time;

    for (uint8_t i = 0; i < cap->modules_size; ++i) {
        p->print(getModuleName(i));
        p->print('\t');
        float load =
            (float)cap->module[i] / ONE_MILLISECOND_mi / total_modules_time;
        p->printf_P(strf_per, load * 100);
        p->println();
    }
    p->print(getStrP(str_system));
    p->print('\t');
    p->printf_P(
        strf_per,
        ((float) (system_time / cap->duration) * 100));
    p->println();
}

}  // namespace Actions

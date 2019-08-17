#pragma once

#include "Actions.h"

namespace Actions {

class ShowLoop : public EmptyParamAction {
   public:
    void exec(Print* p);
};

void ShowLoop::exec(Print* p) {
    Profiler::State state = watchDog.getState();
    switch (state) {
        case Profiler::CAPTURE_IDLE:
            watchDog.startCapture();
            p->print(FPSTR(str_start));
            p->print(FPSTR(str_capture));
            p->printf_P(strf_lu_ms, watchDog.getCaptureTimeLeft());
            p->println();
            break;
        case Profiler::CAPTURE_IN_PROGRESS:
            p->print(FPSTR(str_capturing));
            p->printf_P(strf_lu_ms, watchDog.getCaptureTimeLeft());
            p->print(FPSTR(str_left));
            p->println();
            break;

        case Profiler::CAPTURE_DONE:
            watchDog.printCapture(p);
            watchDog.setIdle();
    };
}

}  // namespace Actions

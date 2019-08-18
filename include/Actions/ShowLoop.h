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
            p->print(getStrP(str_start));
            p->print(getStrP(str_capture));
            p->printf_P(strf_lu_ms, watchDog.getCaptureTimeLeft());
            p->println();
            break;
        case Profiler::CAPTURE_IN_PROGRESS:
            p->print(getStrP(str_capturing));
            p->printf_P(strf_lu_ms, watchDog.getCaptureTimeLeft());
            p->print(getStrP(str_left, false));
            p->println();
            break;
        case Profiler::CAPTURE_DONE:
            watchDog.printCapture(p);
            watchDog.setIdle();
    };
}

}  // namespace Actions

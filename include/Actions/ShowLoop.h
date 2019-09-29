
#pragma once

#include "Actions.h"
#include "App.h"

namespace Actions {

class ShowLoop : public Action {
   public:
    void exec(Print*);
};

void ShowLoop::exec(Print* p) {
    LoopLogger* logger = app.getLoopLogger();
    LoopLoggerState state = logger->getState();
    switch (state) {
        case CAPTURE_IDLE:
            p->print(StrUtils::getStrP(str_start));
            p->print(StrUtils::getStrP(str_capture));
            p->printf_P(strf_lu_ms, logger->getDuration());
            p->println();
            app.getLoopLogger()->start();
            break;
        case CAPTURE_IN_PROGRESS:
            p->print(StrUtils::getStrP(str_capturing));
            p->printf_P(strf_lu_ms, logger->getDuration());
            p->println();
            break;
        case CAPTURE_DONE:
            app.printLoopCapture(p);            
            logger->setIdle();
            break;
    };
}

}  // namespace Actions

#pragma once

#include "Actions.h"

namespace Actions {

class ShowClock : public EmptyParamAction {
   public:
    void exec(Print* p);
};

void ShowClock::exec(Print* p) {
    p->print(FPSTR(str_clock));
    p->print(FPSTR(str_time));
    char buf[32];
    struct tm dateTime = rtc.getDateTime();
    p->print(tmtoa(&dateTime, buf));
    p->println();
}

ShowClock* showClock = new ShowClock();

}  // namespace Actions

#pragma once

#include "ParameterlessAction.h"

namespace Actions {

class ShowClock : public ParameterlessAction {
   public:
    void Execute(Print* p);
    size_t printTo(Print& p) const;
};

inline void ShowClock::Execute(Print* p) {
    p->print(FPSTR(str_clock));
    p->print(FPSTR(str_time));
    char buf[32];
    struct tm dateTime = rtc.getDateTime();
    p->print(tmtoa(&dateTime, buf));
    p->println();
}

inline size_t ShowClock::printTo(Print& p) const {
    size_t res = p.println(F("show clock"));
    return res;
}

ShowClock* showClock = new ShowClock();

}  // namespace Actions

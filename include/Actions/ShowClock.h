#pragma once

#include "Actions.h"

namespace Actions {

class ShowClock : public Action {
   public:
    void exec(Print* p);
};

void ShowClock::exec(Print* p) {
    char buf[32];
    struct tm dateTime = rtc.getDateTime();
    p->println(tmtoa(&dateTime, buf));
}

}  // namespace Actions

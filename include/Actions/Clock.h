#pragma once

#include "Actions.h"
#include "SystemClock.h"

namespace Actions {

class ShowClock : public Action {
   public:
    void exec(Print* p);
};

void ShowClock::exec(Print* p) {    
    char buf[32];
    p->println(TimeUtils::getDateTimeFormated(buf, rtc->getLocal()));    
}

}  // namespace Actions

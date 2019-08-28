#pragma once

#include "Actions.h"
#include "SystemClock.h"

namespace Actions {

class ShowClock : public Action {
   public:
    void exec(Print* p);
};


void ShowClock::exec(Print* p) {    
    p->println(rtc.getDateTimeStr());
}

}  // namespace Actions

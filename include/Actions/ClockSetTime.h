#pragma once

#include "Actions.h"

#include "CommonTypes.h"
#include "TimeUtils.h"

namespace Actions {

class ClockSetTime : public StringAction {
   public:
    ClockSetTime(String param);
    void exec(Print* p);
};

ClockSetTime::ClockSetTime(String param) : StringAction(param) {}

void ClockSetTime::exec(Print* p) {
    tm tm;
    if (TimeUtils::encodeTime(param.c_str(), tm)) {
        DateTime dt = DateTime(tm);
        p->println(dt);
    } else {
        p->print(StrUtils::getStrP(str_invalid));
        p->print(' ');
        p->print(StrUtils::getStrP(str_time));
        p->print(' ');
        p->println(param);
    }
}

}  // namespace Actions

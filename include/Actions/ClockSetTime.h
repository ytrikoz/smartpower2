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
    char paramStr[32];
    if (param.length()) {
        strcpy(paramStr, param.c_str());
    } else {
        p->print(StrUtils::getStrP(str_invalid));
        p->print(StrUtils::getStrP(str_time));
    }
    tm tm;
    if (encodeTime(paramStr, tm)) {
        Time t = Time(tm);
        p->println(t);
    }
}

}  // namespace Actions

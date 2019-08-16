#pragma once

#include "Actions.h"
#include "CommonTypes.h"
#include "TimeUtils.h"

namespace Actions {

class ClockSet : public EmptyParamAction {
   public:
    ClockSet();
    void exec(Print* p);
};

ClockSet::ClockSet(){
    strcpy(name, "test");
}
void ClockSet::exec(Print* p) {
    p->print(FPSTR(str_set));
    p->print(FPSTR(str_date));
    char date_str[32];
    strcpy_P(date_str, str_build_date);
    tm tm;
    if (str_to_date(date_str, tm)) {
        Date d = Date(tm);
        p->println(d);
    } else {
        p->printf_P(str_invalid, str_build_date);
    }
}

}  // namespace Actions

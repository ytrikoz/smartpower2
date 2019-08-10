#pragma once

#include "CommonTypes.h"
#include "TimeUtils.h"

namespace Actions {

class ClockSet : public Printable {
   public:
    void exec(Print* p);
    size_t printTo(Print& p) const;
};

inline void ClockSet::exec(Print* p) {
    p->print(FPSTR(str_set));
    p->print(FPSTR(str_date));

    char date_str[32];
    strcpy_P(date_str, str_build_date);
    tm tm;
    if (str_to_date(date_str, tm)) {
        Date d = Date(tm);
        p->println(d);
    } else {
        p->println(FPSTR(str_invalid));
    }
}

size_t ClockSet::printTo(Print& p) const {
    size_t res = p.println("clock set");
    return res;
}

ClockSet* clockSet = new ClockSet();

}  // namespace Actions

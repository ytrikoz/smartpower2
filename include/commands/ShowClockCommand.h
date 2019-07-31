#pragma once

#include <Arduino.h>

#include "SystemClock.h"
#include "consts.h"
#include "wireless.h"

namespace commands {

class ShowClockCommand : public Printable {
   public:
    void Execute(Print* p);
    size_t printTo(Print& p) const;
};

inline void commands::ShowClockCommand::Execute(Print* p) {
    p->print(FPSTR(str_clock));
    p->print(FPSTR(str_time));    

    char buf[32];
    struct tm dateTime = rtc.getDateTime();
    
    p->print(tmtoa(&dateTime, buf));
    p->printf_P(strf_synced, rtc.isSynced());   
    p->println();
}

inline size_t commands::ShowClockCommand::printTo(Print& p) const {
    size_t res = p.println(F("show clock"));
    return res;
}

}  // namespace commands

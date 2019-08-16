#pragma once

#include "Actions.h"

namespace Actions {

class PowerLog : public NumericAction {
   public:
    PowerLog(const char* str, size_t param);
    void exec(Print* p);
};

PowerLog::PowerLog(const char* str, size_t param) : NumericAction(str, param) {}

void PowerLog::exec(Print* p) {
    DEBUG.printf("PowerLog(%d)", param);
    DEBUG.println();
    if (psuLog->empty()) {
        p->println(FPSTR(str_empty));
    } else {
        size_t num = (size_t)param <= psuLog->size() ? param : psuLog->size();
        psuLog->printLast(p, num);
    }
}

}  // namespace Actions
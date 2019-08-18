#pragma once

#include "Actions.h"

namespace Actions {

class PowerLog : public NumericAction {
   public:
    PowerLog(size_t param);
    void exec(Print* p);
};

PowerLog::PowerLog(size_t param) : NumericAction(param) {}

void PowerLog::exec(Print* p) {
    if (psuLog->empty()) {
        p->println(getStrP(str_empty, false));
    } else {
        size_t num = (size_t) param <= psuLog->size() ? param : psuLog->size();
        psuLog->printLast(p, num);
    }
}

}  // namespace Actions
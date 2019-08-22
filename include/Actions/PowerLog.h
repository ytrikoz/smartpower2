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
        psuLog->printLast(p, param);
    }
}

}  // namespace Actions
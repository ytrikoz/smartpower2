#pragma once

#include "Actions.h"

namespace Actions {

class ShowStatus : public EmptyParamAction {
   public:
    void exec(Print* p);
    size_t printTo(Print& p) const;
};

inline void ShowStatus::exec(Print* p) {
    p->println(getHeapStat());
    loopWD.printDiag(p);
}

inline size_t ShowStatus::printTo(Print& p) const {
    size_t res = p.println("show status");
    return res;
}

ShowStatus* showStatus = new ShowStatus();

}  // namespace Actions

#pragma once

#include "Actions.h"

namespace Actions {

class ShowStatus : public EmptyParamAction {
   public:
    void exec(Print* p);
};

inline void ShowStatus::exec(Print* p) {
    p->println(getHeapStat());
    loopWD.printDiag(p);
}

ShowStatus* showStatus = new ShowStatus();

}  // namespace Actions

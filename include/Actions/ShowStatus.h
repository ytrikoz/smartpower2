#pragma once

#include "Actions.h"

namespace Actions {

class ShowStatus : public EmptyParamAction {
   public:
    void exec(Print* p);
};

void ShowStatus::exec(Print* p) {
    p->println(getHeapStat());
}

}  // namespace Actions

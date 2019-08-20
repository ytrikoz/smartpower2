#pragma once

#include "Actions.h"

namespace Actions {

class ShowStatus : public Action {
   public:
    void exec(Print* p);
};

void ShowStatus::exec(Print* p) {
    p->println(getHeapStat());
}

}  // namespace Actions

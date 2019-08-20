#pragma once

#include "Actions.h"

namespace Actions {

class PowerOn : public Action {
   public:
    void exec(Print* p);
};

void PowerOn::exec(Print* p) { psu->setState(POWER_ON); }

}  // namespace Actions

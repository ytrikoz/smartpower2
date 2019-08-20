#pragma once

#include "Actions.h"

namespace Actions {

class PowerOff : public Action {
   public:
    void exec(Print* p);
};

void PowerOff::exec(Print* p) { psu->setState(POWER_OFF); }

}  // namespace Actions

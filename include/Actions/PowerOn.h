#pragma once

#include "Actions.h"

namespace Actions {

class PowerOn : public Action {
   public:
    void exec(Print* p);
};

void PowerOn::exec(Print* p) {
    if (psu->getState() == POWER_OFF) psu->togglePower();
}

}  // namespace Actions

#pragma once

#include "Actions.h"

namespace Actions {

class PowerOff : public Action {
   public:
    void exec(Print* p);
};

void PowerOff::exec(Print* p) {
    if (psu->getState() == POWER_ON) psu->togglePower();
}    

}  // namespace Actions

#pragma once

#include "Actions.h"

namespace Actions {

class PowerOn : public Action {
   public:
    void exec(Print* p);
};

void PowerOn::exec(Print* p) {
    if (app.getPsu()->getState() == POWER_OFF) app.getPsu()->togglePower();
}

}  // namespace Actions

#pragma once

#include "Actions.h"

namespace Actions {

class PowerOff : public Action {
  public:
    void exec(Print *p);
};

void PowerOff::exec(Print *p) {
    if (app.getPsuState()->getPower(POWER_ON))
        app.getPsu()->togglePower();
}

} // namespace Actions

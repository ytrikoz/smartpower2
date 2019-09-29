#pragma once

#include "Actions.h"

namespace Actions {

class ShowClock : public Action {
   public:
    void exec(Print* p);
};

void ShowClock::exec(Print* p) { app.getInstance(MOD_CLOCK)->printDiag(p); };

}  // namespace Actions

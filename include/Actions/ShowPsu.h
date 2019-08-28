#pragma once

#include "Actions.h"
#include "Psu.h"

namespace Actions {

class ShowPsu : public Action {
   public:
    void exec(Print* p);
};

void ShowPsu::exec(Print* p) {    
    psu->printDiag(p);
}

}  // namespace Actions

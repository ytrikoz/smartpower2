#pragma once

#include "Actions.h"
#include "Psu.h"

namespace Actions {

class ShowPsu : public Action {
   public:
    void exec(Print* p);
};

void ShowPsu::exec(Print* p) {    
    if (psu) {
        psu->printDiag(p);
    } else {
        p->println(StrUtils::getStrP(str_disabled, false));
    }
}

}  // namespace Actions

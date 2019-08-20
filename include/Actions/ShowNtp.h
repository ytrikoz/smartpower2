#pragma once

#include "Actions.h"

namespace Actions {

class ShowNtp : public Action {
   public:
    void exec(Print* p);
};

void ShowNtp::exec(Print* p) {
    if (ntp) {
        ntp->printDiag(p);
    } else {
        p->println(getStrP(str_disabled));
    }
}

}  // namespace Actions
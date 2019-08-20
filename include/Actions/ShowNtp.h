#pragma once

#include "Actions.h"

namespace Actions {

class ShowNtp : public EmptyParamAction {
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
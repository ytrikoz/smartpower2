#pragma once

#include "Actions.h"

namespace Actions {

class ShowNtp : public EmptyParamAction {
   public:
    void exec(Print* p);
};

void ShowNtp::exec(Print* p) {
    p->print(FPSTR(str_ntp));
    if (ntp) {
        ntp->printDiag(p);
    } else {
        p->print(FPSTR(str_disabled));
    }
    p->println();
}

}  // namespace Actions
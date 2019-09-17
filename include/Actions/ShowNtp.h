#pragma once

#include "Actions.h"

namespace Actions {

class ShowNtp : public Action {
   public:
    void exec(Print* p);
};

void ShowNtp::exec(Print* p) {
    if (ntp) {
        Print* old = ntp->getOutput();
        ntp->setOutput(p);
        ntp->printDiag();
        ntp->setOutput(old);
    } else {
        p->println(StrUtils::getStrP(str_disabled));
    }
}

}  // namespace Actions
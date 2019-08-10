#pragma once

#include "Actions.h"

namespace Actions {

class ShowNtp : public EmptyParamAction {
   public:
    void exec(Print* p);
    size_t printTo(Print& p) const;
};

inline void ShowNtp::exec(Print* p) {
    p->print(FPSTR(str_ntp));
    if (ntp) {
        ntp->printDiag(p);
    } else {
        p->print(FPSTR(str_disabled));
    }
    p->println();
}

inline size_t ShowNtp::printTo(Print& p) const {
    size_t res = p.println("show ntp");
    return res;
}

ShowNtp* showNtp = new ShowNtp();
}  // namespace Actions
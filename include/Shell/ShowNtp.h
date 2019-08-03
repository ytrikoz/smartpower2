#pragma once

#include "ParameterlessCommand.h"

namespace shell {

class ShowNtp : public ParameterlessCommand {
   public:
    void Execute(Print* p);
    size_t printTo(Print& p) const;
};

inline void ShowNtp::Execute(Print* p) {
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
}  // namespace shell
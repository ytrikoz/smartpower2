#pragma once

#include <Arduino.h>
#include "consts.h"
#include "global.h"

namespace commands {

class ShowNtpCommand : public Printable {
   public:
    void Execute(Print *p);
    size_t printTo(Print &p) const;
};

inline void ShowNtpCommand::Execute(Print* p) {
    p->printf_P(str_ntp);
    if (ntp) {
        ntp->printDiag(p);    
    } else {
        p->print(FPSTR(str_disabled));
    }
    p->println();
}

inline size_t ShowNtpCommand::printTo(Print& p) const {
    size_t res = p.println("show ntp");
    return res;
}

}
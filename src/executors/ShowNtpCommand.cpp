#include "executors/ShowNtpCommand.h"

#include "consts.h"
#include "global.h"

namespace executors {

void ShowNtpCommand::Execute(Print* p) {
    p->printf_P(str_ntp);
    if (ntp) {
        ntp->printDiag(p);    
    } else {
        p->print(FPSTR(str_disabled));
    }
    p->println();
}

size_t ShowNtpCommand::printTo(Print& p) const {
    size_t res = p.println("show ntp");
    return res;
}

}  // namespace executors
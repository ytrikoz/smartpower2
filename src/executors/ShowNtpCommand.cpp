#include "executors/ShowNtpCommand.h"

#include "consts.h"
#include "global.h"

namespace executors {

void ShowNtpCommand::Execute(Print *p)
{
    if(ntp) {
        p->printf_P(str_ntp);
        ntp->printDiag(p);
        p->println();
    } else
    {
        p->printf_P(str_ntp);
        p->printf_P(str_disabled);
        p->println();
    }
}

size_t ShowNtpCommand::printTo(Print& p) const {
    size_t res = p.println("show ntp");
    return res;
}

}
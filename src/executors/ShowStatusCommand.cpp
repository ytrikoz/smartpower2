#include "executors/ShowStatusCommand.h"

#include "consts.h"
#include "global.h"

namespace executors {

void ShowStatusCommand::Execute(Print *p)
{
    p->println(getLoopStat());
    p->println(getHeapStat());
}

size_t ShowStatusCommand::printTo(Print& p) const {
    size_t res = p.println("show status");
    return res;
}

}
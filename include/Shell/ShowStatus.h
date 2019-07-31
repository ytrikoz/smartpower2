#pragma once

#include "ParameterlessCommand.h"

namespace shell {

class ShowStatus : public ParameterlessCommand {
   public:
    void Execute(Print* p);
    size_t printTo(Print& p) const;
};

inline void ShowStatus::Execute(Print* p) {
    p->println(getLoopStat());
    p->println(getHeapStat());
}

inline size_t ShowStatus::printTo(Print& p) const {
    size_t res = p.println("show status");
    return res;
}

ShowStatus* showStatus = new ShowStatus();

}  // namespace shell

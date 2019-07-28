#pragma once

#include <Arduino.h>

namespace commands {

class ShowStatusCommand : public Printable {
   public:
    void Execute(Print *p);
    size_t printTo(Print &p) const;
};

inline void commands::ShowStatusCommand::Execute(Print* p) {
    p->println(getLoopStat());
    p->println(getHeapStat());
}

inline  size_t commands::ShowStatusCommand::printTo(Print& p) const {
    size_t res = p.println("show status");
    return res;
}

}  // namespace executors

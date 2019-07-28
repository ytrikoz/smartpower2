#pragma once

#include <Arduino.h>

#include "wireless.h"

namespace commands {

class ShowDiagCommand : public Printable {
   public:
    void Execute(Print *p);
    size_t printTo(Print &p) const;
};

inline void commands::ShowDiagCommand::Execute(Print* p) {
    wireless::printDiag(p);
}

inline  size_t commands::ShowDiagCommand::printTo(Print& p) const {
    size_t res = p.println("show diag");
    return res;
}

}  // namespace executors

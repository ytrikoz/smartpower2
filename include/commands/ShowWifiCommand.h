#pragma once

#include <Arduino.h>

#include "wireless.h"

namespace commands {

class ShowWifiCommand : public Printable {
   public:
    void Execute(Print *p);
    size_t printTo(Print &p) const;
};

inline void commands::ShowWifiCommand::Execute(Print* p) {
    wireless
}

inline  size_t commands::ShowWifiCommand::printTo(Print& p) const {
    size_t res = p.println("show wifi");
    return res;
}

}  // namespace executors

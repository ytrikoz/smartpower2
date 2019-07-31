#pragma once

#include <Arduino.h>
#include "consts.h"
#include "global.h"
#include "wireless.h"

namespace shell {

class ParameterlessCommand : public Printable {
   public:
    virtual void Execute(Print* p);
    virtual size_t printTo(Print& p) const;
};

}  // namespace shell

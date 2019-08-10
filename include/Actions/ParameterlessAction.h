#pragma once

#include <Arduino.h>

#include "Global.h"

namespace Actions {

class ParameterlessAction : public Printable {
   public:
    virtual void Execute(Print* p);
    virtual size_t printTo(Print& p) const;
};

}  // namespace Actions

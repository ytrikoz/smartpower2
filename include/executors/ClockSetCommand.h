#pragma once

#include <Arduino.h>
#include "Types.h"
namespace executors {

class ClockSetCommand : public Printable {
   public:
    void Execute(Print *p);
    size_t printTo(Print &p) const;        
};

}
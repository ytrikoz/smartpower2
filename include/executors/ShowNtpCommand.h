#pragma once

#include <Arduino.h>

namespace executors {

class ShowNtpCommand : public Printable {
   public:
    void Execute(Print *p);
    size_t printTo(Print &p) const;
};

}
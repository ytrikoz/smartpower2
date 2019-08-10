#pragma once

#include <Arduino.h>

#include "Global.h"

namespace Actions {

class EmptyParamAction : public Printable {
   public:
    virtual void exec(Print* p);
    virtual size_t printTo(Print& p) const;
};

class NumericAction: public EmptyParamAction {
    public:
    virtual void setParam(long number);
    virtual void exec(Print* p);
    virtual size_t printTo(Print& p) const;
};

}  // namespace Actions

#pragma once

#include <Arduino.h>

#include "Global.h"

namespace Actions {

class EmptyParamAction {
   public:
    EmptyParamAction();
};

EmptyParamAction::EmptyParamAction() {}

class NumericAction : public EmptyParamAction {
   public:
    NumericAction(size_t param);
    virtual void exec(Print* p);

   protected:
    int param;
};

NumericAction::NumericAction(size_t param) : EmptyParamAction() {
    this->param = param;
}

class StringAction : EmptyParamAction {
   public:
    StringAction(String param);
    virtual void exec(Print* p);

   protected:
    String param;
};

StringAction::StringAction(String param) : EmptyParamAction(){};

}  // namespace Actions

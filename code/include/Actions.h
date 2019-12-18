#pragma once

#include "Strings.h"

namespace Actions {


class Action {
   public:
    virtual void exec(Print *p) = 0;
};

class NumericAction : public Action {
   public:
    NumericAction(size_t param);
    virtual void exec(Print *p) = 0;

   protected:
    int param;
};

class StringAction : public Action {
   public:
    StringAction(String param);
    virtual void exec(Print *p) = 0;

   protected:
    String param;
};

NumericAction::NumericAction(size_t param) { this->param = param; };

StringAction::StringAction(String param) { this->param = param; };

}  // namespace Actions

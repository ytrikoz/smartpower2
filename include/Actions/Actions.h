#pragma once

#include <Arduino.h>

#include "Global.h"

namespace Actions {

class EmptyParamAction : public Printable {
   public:
    EmptyParamAction();
    EmptyParamAction(const char* name);
    char* getName();
    size_t printTo(Print& p) const;
   protected:
    char* name;
};

EmptyParamAction::EmptyParamAction() { this->name = new char[16]{0}; }

EmptyParamAction::EmptyParamAction(const char* str) {
    EmptyParamAction();
    StrUtils::setstr(this->name, str, 16);
}
char* EmptyParamAction::getName() { return name; }

size_t EmptyParamAction::printTo(Print& p) const {
    size_t res = p.println(name);
    return res;
}

class NumericAction : public EmptyParamAction {
   public:
    NumericAction(const char* str, size_t param);
    virtual void exec(Print* p);

   protected:
    int param;
};

NumericAction::NumericAction(const char* str, size_t param)
    : EmptyParamAction(str) {
    this->param = param;
}

}  // namespace Actions

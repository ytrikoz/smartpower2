#pragma once

#include <Arduino.h>

#include "Global.h"

namespace Actions {

class EmptyParamAction {
   public:
    EmptyParamAction();
    EmptyParamAction(const char* name);
   protected:
    char* name;
};

EmptyParamAction::EmptyParamAction() { this->name = new char[16]{0}; }

EmptyParamAction::EmptyParamAction(const char* str) {
    EmptyParamAction();
    StrUtils::setstr(this->name, str, 16);
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

class StringAction : EmptyParamAction { 
    public:
    StringAction(const char* str, String param);
    virtual void exec(Print* p);
    protected:
    String param;
};

StringAction::StringAction(const char* str, String param) : EmptyParamAction(str) {};

}  // namespace Actions

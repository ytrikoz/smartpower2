#pragma once

#include "Actions.h"

namespace Actions {

class Backlight : public NumericAction {
   public:
    Backlight(size_t param);
    void exec(Print* p);
};

Backlight::Backlight(size_t param) : NumericAction(param) {}

void Backlight::exec(Print* p) {
    bool enabled = (param > 0);
    p->print(StrUtils::getStrP(str_lcd));
    p->print(StrUtils::getStrP(str_backlight));
    p->println(StrUtils::getStrP(enabled ? str_on: str_off));
    if (enabled)
        display->backlightOn();
    else
        display->backlightOff();
}

}  // namespace Actions
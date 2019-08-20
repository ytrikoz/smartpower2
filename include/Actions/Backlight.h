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
    p->print(getStrP(str_lcd));
    p->print(getStrP(str_backlight));
    p->print(enabled ? getStrP(str_on) : getStrP(str_off));
    if (enabled)
        display->enableBacklight();
    else
        display->enableBacklight();
}

}  // namespace Actions
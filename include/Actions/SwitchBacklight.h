#pragma once

#include "Actions.h"

namespace Actions {

class SwitchBacklight : public NumericAction {
   public:
    SwitchBacklight(size_t param);
    void exec(Print* p);
};

SwitchBacklight::SwitchBacklight(size_t param) : NumericAction(param) {}

void SwitchBacklight::exec(Print* p) {
    bool enabled = (param > 0);
    p->print(StrUtils::getStrP(str_backlight));
    if (enabled)
        app.getDisplay()->backlightOn();        
    else
        app.getDisplay()->backlightOff();
    p->println(StrUtils::getEnabledStr(enabled));

}

}  // namespace Actions
#pragma once

#include "Actions.h"

namespace Actions {

class ShowWifi : public EmptyParamAction {
   public:
    void exec(Print* p);
};

void ShowWifi::exec(Print* p) {
    p->println(Wireless::wifiModeInfo());

    p->println(Wireless::hostIPInfo().c_str());

    p->print(getStrP(str_network));    
    if (Wireless::hasNetwork()) {
        p->println(getStrP(str_up, false));
    } else {
        p->println(getStrP(str_down, false));;
    }
}

}  // namespace Actions

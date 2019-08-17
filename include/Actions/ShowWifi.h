#pragma once

#include "Actions.h"

namespace Actions {

class ShowWifi : public EmptyParamAction {
   public:
    void exec(Print* p);
};

void ShowWifi::exec(Print* p) {
    p->print(FPSTR(str_wifi_));
    switch (Wireless::getWirelessMode()) {
        case WLAN_OFF:
            p->println(FPSTR(str_off));
            return;
        case WLAN_AP:
            p->print(FPSTR(str_ap));
            break;
        case WLAN_STA:
            p->print(FPSTR(str_sta));
            break;
        case WLAN_AP_STA:
            p->print(str_ap);
            p->print(str_sta);
            break;
    }
    p->print(FPSTR(str_mode));
    p->println();

    p->print(FPSTR(str_network));
    if (Wireless::hasNetwork()) {
        p->println(FPSTR(str_up));
    } else {
        p->println(FPSTR(str_down));
    }

    p->println(Wireless::hostIPInfo().c_str());
}

}  // namespace Actions

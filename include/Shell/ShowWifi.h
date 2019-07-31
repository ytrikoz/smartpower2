#pragma once

#include "ParameterlessCommand.h"

namespace shell {

class ShowWifi : public ParameterlessCommand {
   public:
    void Execute(Print* p);
    size_t printTo(Print& p) const;
};

inline void ShowWifi::Execute(Print* p) {
    p->print(FPSTR(str_wifi_));
    switch (wireless::getWirelessMode()) {
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
    if (wireless::hasNetwork()) {
        p->println(FPSTR(str_up));
    } else {
        p->println(FPSTR(str_down));
    }

     p->println(wireless::hostIPInfo().c_str());
}

inline size_t ShowWifi::printTo(Print& p) const {
    size_t res = p.println("show wifi");
    return res;
}

ShowWifi* showWifi = new ShowWifi();

}  // namespace shell

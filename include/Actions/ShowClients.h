#pragma once

#include "Actions.h"

namespace Actions {

class ShowClients : public Action {
   public:
    void exec(Print* p);
};

void ShowClients::exec(Print* p) {
    if ((Wireless::getMode() != Wireless::WLAN_STA) &&
        !getConnectedStationInfo().equals("")) {
        p->print(StrUtils::getIdentStrP(str_wifi));
        p->print(getConnectedStationInfo().c_str());
    }
    p->print(StrUtils::getStrP(str_http));
    p->println(get_http_clients_count());

    p->print(StrUtils::getStrP(str_telnet));
    p->println(get_telnet_clients_count());
}

}  // namespace Actions
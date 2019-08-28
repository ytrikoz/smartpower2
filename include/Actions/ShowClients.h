#pragma once

#include "Actions.h"

namespace Actions {

class ShowClients : public Action {
   public:
    void exec(Print* p);
};

void ShowClients::exec(Print* p) {
    if ((Wireless::getWirelessMode() != WLAN_STA) &&
        !getConnectedStationInfo().equals("")) {
        p->print(getIdentStrP(str_wifi));
        p->print(getConnectedStationInfo().c_str());
    }
    p->print(getStrP(str_http));
    p->println(get_http_clients_count());

    p->print(getStrP(str_telnet));
    p->println(get_telnet_clients_count());
}

}  // namespace Actions
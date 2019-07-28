#pragma once

#include <Arduino.h>
#include "consts.h"
#include "global.h"

namespace commands {

class ShowClientsCommand : public Printable {
   public:
    void Execute(Print *p);
    size_t printTo(Print &p) const;
};


inline void commands::ShowClientsCommand::Execute(Print* p) {
	char buf[OUTPUT_MAX_LENGTH];
	if (wireless::getWirelessMode() != WLAN_STA)
	{
    	sprintf(buf, "wifi clients: %s", getConnectedStationInfo().c_str());
    	p->println(buf);
	}
    sprintf(buf, "http: %d", get_http_clients_count());
    p->println(buf);
	sprintf(buf, "telnet: %d", get_telnet_clients_count());
    p->println(buf);
}

inline size_t commands::ShowClientsCommand::printTo(Print& p) const {
    size_t res = p.println(F("show clients"));
    return res;
}

}
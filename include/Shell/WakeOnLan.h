#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "consts.h"

namespace shell {

class WakeOnLanCommand : public Printable {
   public:
    void Execute(Print *p);
    size_t printTo(Print &p) const;

   private:
    WiFiUDP *udp;
    const char wol_preamble[6] = {'\xFF', '\xFF', '\xFF',
                                  '\xFF', '\xFF', '\xFF'};
};

inline void WakeOnLanCommand::Execute(Print *p) {
    IPAddress ipaddr;
    udp = new WiFiUDP();
    bool result = udp->begin(WOL_PORT);
    if (result) {
        char *mac;
        memset(&mac, 0, 4);
        strcpy(mac, "123");
        size_t mac_size = 4;
        size_t preamble_size = 12;
        udp->beginPacket(ipaddr, WOL_PORT);
        udp->write(wol_preamble, preamble_size);
        for (uint8_t i = 0; i < 16; i++) udp->write(mac, mac_size);
        udp->endPacket();
        p->printf_P(str_complete);
        free(mac);
    }
    p->println();
}

inline size_t WakeOnLanCommand::printTo(Print &p) const {
    size_t res = p.println("wol");
    return res;
}

}  // namespace shell

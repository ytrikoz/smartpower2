#pragma once

#include "Actions.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "PrintUtils.h"
#include "StrUtils.h"
#include "Strings.h"

namespace Actions {

class WakeOnLan {
   public:
    WakeOnLan(Print *p);
    void exec(String ipaddr, String mac);

   private:
    const char wol_preamble[6] = {'\xFF', '\xFF', '\xFF',
                                  '\xFF', '\xFF', '\xFF'};
    Print *out;
};

WakeOnLan::WakeOnLan(Print *p) { out = p; }

void WakeOnLan::exec(String ipStr, String macStr) {
    IPAddress ipaddr;
    if (!ipaddr.fromString(ipStr)) {
        PrintUtils::println(out, FPSTR(str_wrong), FPSTR(str_ip),
                            ipStr.c_str());
        return;
    }
    uint8_t mac[6];
    if (!StrUtils::atomac(macStr.c_str(), mac)) {
        PrintUtils::println(out, FPSTR(str_wrong), FPSTR(str_mac),
                            macStr.c_str());
        return;
    }
    WiFiUDP *udp = new WiFiUDP();
    bool result = udp->begin(WOL_PORT);
    if (result) {
        size_t preamble_size = 12;
        if (udp->beginPacket(ipaddr, WOL_PORT)) {
            udp->write(wol_preamble, preamble_size);
            for (uint8_t i = 0; i < 16; i++)
                udp->write(mac, 6);
            udp->endPacket();
        } else {
            PrintUtils::println(out, FPSTR(str_error));
        }
    }
    udp->stop();
    free(udp);
}  // namespace Actions

}  // namespace Actions

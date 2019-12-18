#pragma once

#include "Actions.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "Strings.h"
#include "Utils/PrintUtils.h"
#include "Utils/StrUtils.h"

namespace Actions {

class WakeOnLan {
   public:
    WakeOnLan(Print *p);
    void exec(const String& ip, const String& mac);

   private:
    const char wol_preamble[6] = {'\xFF', '\xFF', '\xFF',
                                  '\xFF', '\xFF', '\xFF'};
    Print *out;
};

WakeOnLan::WakeOnLan(Print *p) { out = p; }

void WakeOnLan::exec(const String& ip, const String& mac) {
    IPAddress ipaddr;
    if (!ipaddr.fromString(ip)) {
        PrintUtils::print(out, FPSTR(str_wrong), FPSTR(str_ip), mac.c_str());
        PrintUtils::println(out);
        return;
    }
    uint8_t mac_[6];
    if (!StrUtils::atomac(mac.c_str(), mac_)) {
        PrintUtils::print(out, FPSTR(str_wrong), FPSTR(str_mac), mac.c_str());
        PrintUtils::println(out);
        return;
    }
    WiFiUDP *udp = new WiFiUDP();
    bool result = udp->begin(WOL_PORT);
    if (result) {
        size_t preamble_size = 12;
        if (udp->beginPacket(ipaddr, WOL_PORT)) {
            udp->write(wol_preamble, preamble_size);
            for (uint8_t i = 0; i < 16; i++)
                udp->write(mac_, 6);
            udp->endPacket();
        } else {
            PrintUtils::println(out, FPSTR(str_error));
        }
    }
    udp->stop();
    free(udp);
}  // namespace Actions

}  // namespace Actions

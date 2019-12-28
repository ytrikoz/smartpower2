#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "Consts.h"
#include "Utils/PrintUtils.h"
#include "Utils/StrUtils.h"

namespace Actions {

class WakeOnLan {
   public:
   static void send(Print *out, const IPAddress ipaddr, const uint8_t macaddr[6]) {
        const char preamble[6] = {'\xFF', '\xFF', '\xFF',
                                      '\xFF', '\xFF', '\xFF'};
        WiFiUDP *udp = new WiFiUDP();
        bool result = udp->begin(WOL_PORT);
        if (result) {
            size_t preamble_size = 12;
            if (udp->beginPacket(ipaddr, WOL_PORT)) {
                udp->write(preamble, preamble_size);
                for (uint8_t i = 0; i < 16; i++)
                    udp->write(macaddr, 6);
                udp->endPacket();
            } else {
                PrintUtils::print(out, FPSTR(str_error), FPSTR(str_network));
                PrintUtils::println(out);
            }
        }
        udp->stop();
        free(udp);
    }

    static void send(Print *out, const String &host, const String &mac) {
        IPAddress ip_addr;
        if (StrUtils::isip(host.c_str())) {
            if (!ip_addr.fromString(host)) {
                PrintUtils::print(out, FPSTR(str_error), FPSTR(str_ipaddr), host);
                PrintUtils::println(out);
                return;
            }
        } else {
            if (!WiFi.hostByName(host.c_str(), ip_addr)) {
                PrintUtils::print(out, FPSTR(str_error), FPSTR(str_dns_resolve), host);
                PrintUtils::println(out);
                return;
            }
        }
        uint8_t mac_addr[6];
        if (!StrUtils::str2mac(mac.c_str(), mac_addr)) {
            PrintUtils::print(out, FPSTR(str_error), FPSTR(str_mac), mac);
            PrintUtils::println(out, FPSTR(str_error));
            return;
        }
        send(out, ip_addr, mac_addr);
    }

    
};

}  // namespace Actions

#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

namespace executors {

class WakeOnLanCommand : public Printable {
   public:
    void Execute(Print *p);
    size_t printTo(Print &p) const;
   private:
    WiFiUDP *udp;
    const char wol_preamble[6] = {'\xFF', '\xFF', '\xFF', '\xFF', '\xFF', '\xFF'};
};

}  // namespace executors

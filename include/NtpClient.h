#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "CommonTypes.h"
#include "Config.h"
#include "StrUtils.h"

#define NTP_PACKET_SIZE 48

struct EpochTime {
   public:
    EpochTime() {
        epoch = 0;
        when = 0;
    }
    EpochTime(unsigned long epoch) {
        this->epoch = epoch;
        this->when = millis();
    }
    unsigned long get() {
        if (epoch > 0) return epoch + (millis_passed(when, millis()) / ONE_SECOND_ms); else return 0;
    }

   private:
    unsigned long epoch;
    unsigned long when;
};

typedef std::function<void(EpochTime&)> NtpClientEventHandler;

class NtpClient {
   public:
    NtpClient();
    void setConfig(Config* config);
    void setOutput(Print* p);
    void setOnTimeSynced(NtpClientEventHandler);
    void setInterval(uint16_t time_s);
    void setServer(const char* server);
    void setZone(uint8_t zone);

    bool begin();
    void end();
    void loop();

    void printDiag(Print* p);

   private:
    void init();
    void sync();
    char* server;
    uint16_t port;
    bool active;
    unsigned long syncInterval;
    unsigned long lastUpdated;
    EpochTime epoch;
    WiFiUDP* udp;
    NtpClientEventHandler onTimeSynced;
    WiFiEventHandler onDisconnected, onGotIp;
    Print* output = &USE_SERIAL;
};
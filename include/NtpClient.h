#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "CommonTypes.h"
#include "Config.h"
#include "StrUtils.h"

#define NTP_PACKET_SIZE 48

typedef std::function<void(EpochTime&)> NtpClientEventHandler;

class NtpClient {
   public:
    NtpClient();
    void setConfig(Config* config);
    void setOutput(Print* p);
    void setInterval(uint16_t time_s);
    void setServer(const char* server);
    void setZone(uint8_t zone);
    bool begin();
    void end();
    void loop();
    void printDiag(Print* p);
    void setOnResponse(NtpClientEventHandler handler);

   private:
    void init();
    void sync();
    char* server;
    uint16_t port;
    bool active;
    unsigned long syncInterval;
    unsigned long lastUpdated;
    EpochTime epochTime;
    WiFiUDP* udp;
    NtpClientEventHandler onResponse;
    WiFiEventHandler onDisconnected, onGotIp;
    Print* output = &USE_SERIAL;
};
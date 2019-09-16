#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "AppModule.h"

#define NTP_PACKET_SIZE 48

typedef std::function<void(EpochTime&)> NtpClientEventHandler;

class NtpClient : public AppModule {
   public:
    NtpClient();
    void setConfig(Config* config);
    void setInterval(uint16_t time_s);
    void setServer(const char* server);
    bool begin();
    void end();
    void loop();
    void printDiag(Print* p);
    void setOnResponse(NtpClientEventHandler handler);

   private:
    void init();
    void sync();

   private:
    bool active;
    char* timeServerPool;
    uint16_t port;
    unsigned long syncInterval;
    unsigned long lastUpdated;
    EpochTime epochTime;
    WiFiUDP* udp;
    NtpClientEventHandler responseHandler;
    WiFiEventHandler onDisconnected, onGotIp;
};
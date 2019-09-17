#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "AppModule.h"

#define NTP_PACKET_SIZE 48

typedef std::function<void(EpochTime&)> EpochTimeEventHandler;

class NtpClient : public AppModule {
   public:
    NtpClient();
    void setConfig(Config* config);
    void loop();
    void printDiag();    
   public:
    void setOnResponse(EpochTimeEventHandler);
    void setInterval(uint16_t time_s);
    void setServer(const char* server);       
   private:
    void sync();
   private:
    bool active;
    char* timeServerPool;
    unsigned long syncInterval;
    unsigned long lastUpdated;
    EpochTime epochTime;
    WiFiUDP* udp;
    EpochTimeEventHandler onResponse;
    WiFiEventHandler onDisconnected, onGotIp;
};
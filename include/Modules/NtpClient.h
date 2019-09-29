#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "AppModule.h"

#define NTP_PACKET_SIZE 48

class NtpClient : public AppModule {
   public:
    NtpClient();
    void loop();
    bool begin();
    size_t printDiag(Print*);
   protected:
    void setConfig(Config* config);

   public:
    void setOnResponse(EpochTimeEventHandler);
    void setInterval(uint16_t time_s);
    void setPoolServer(const char* server);

   private:
    void sync();

   private:
    void start();
    void stop();
    bool active;
    char* timeServerPool;
    unsigned long syncInterval;
    unsigned long lastUpdated;
    EpochTime epochTime;
    WiFiUDP* udp;
    EpochTimeEventHandler onResponse;
};
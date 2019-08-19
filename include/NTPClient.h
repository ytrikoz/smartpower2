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
    void setOnSynced(NtpClientEventHandler);
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
    void send_udp_packet();

    uint8_t buffer[NTP_PACKET_SIZE];
    char server[PARAM_STR_SIZE + 1];
    uint16_t port;
    bool active;
    bool synced;
    unsigned long interval_ms;
    unsigned long lastUpdated;
    EpochTime epoch;
    WiFiUDP* udp;
    NtpClientEventHandler onTimeSynced;
    WiFiEventHandler onDisconnected, onGotIp;
    Print* output = &USE_SERIAL;
};
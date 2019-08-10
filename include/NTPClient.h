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
    bool begin();
    void end();
    void loop();
    void setConfig(Config* config);
    void setOnTimeSynced(NtpClientEventHandler);
    void setSyncInterval(uint16_t time_s);
    void setTimeServer(const char* server);
    void setTimeZone(uint8_t zone);
    void setOutput(Print* p);
    void printDiag(Print* p);

   private:
    void init();
    void sync();
    void send_udp_packet();

    byte buffer[NTP_PACKET_SIZE];
    char server[PARAM_STR_SIZE + 1];
    int port;
    bool active;
    bool initialized;
    unsigned long interval_ms;
    unsigned long updated_ms;
    EpochTime epochTime;
    WiFiUDP* udp;
    NtpClientEventHandler onTimeSynced;
    WiFiEventHandler onDisconnected, onGotIp;

    Print* output = &USE_SERIAL;
};
#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "Config.h"
#include "consts.h"
#include "str_utils.h"
#include "types.h"

// NTP time stamp is in the first 48 bytes of the message
#define NTP_PACKET_SIZE 48

typedef std::function<void(EpochTime &)> NtpClientEventHandler;

class NTPClient {
   public:
    NTPClient();
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
    char server[STR_SIZE + 1];
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
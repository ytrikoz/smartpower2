#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "AppModule.h"

#define NTP_PACKET_SIZE 48

class NtpClient : public AppModule {
  public:
    NtpClient();
    bool begin();
    void start();
    void stop();
    void loop();
    size_t printDiag(Print *);

  protected:
    void setConfig(Config *config);

  public:
    void setOnResponse(TimeEventHandler);
    void setInterval(uint16_t time_s);
    void setPoolServer(const char *server);
    void setTimeout(uint16_t time_ms);

  private:
    void sendRequest();
    void waitResponse();

  private:
    unsigned long epoch_s;
    unsigned requestTime, responseTime;
    WiFiUDP *udp;
    bool active;
    unsigned long timeout;
    char *timeServerPool;
    unsigned long syncInterval;
    TimeEventHandler responseHandler;
};
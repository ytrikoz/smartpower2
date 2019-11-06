#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "AppModule.h"

#define NTP_PACKET_SIZE 48

struct NtpTime : Printable {
    unsigned long epoch = 0;
    unsigned long when = 0;

  public:
    void set(const unsigned long epoch, const unsigned long when) {
        this->epoch = epoch;
        this->when = when;
    }
    unsigned long now() const {
        return epoch ? epoch + millis_since(when) / ONE_SECOND_ms : 0;
    }
    virtual size_t printTo(Print &p) const {
        String str = FPSTR(str_unset);
        if (now())
            str = String(now());
        return PrintUtils::print_nameP_value(&p, str_epoch, str);
    }
};

class NtpClient : public AppModule {
  public:
    NtpClient();
    bool begin();
    //
    bool start();
    void stop();
    //
    void loop();
    //
    size_t printDiag(Print *);

  protected:
    void setConfig(Config *config);

  public:
    void setServer(const char *server);
    void setServer(const char *str, uint16_t port);
    void setRemotePort(int16_t port);
    void setLocalPort(int16_t port);
    void setInterval(uint16_t time);

    void setOnResponse(TimeHandler);

  private:
    void sendRequest();
    void checkResponse();
    void gotResponse(unsigned long epoch);

  private:
    bool enabled = false;
    bool active = false;
    TimeHandler timeHandler;
    WiFiUDP *udp = NULL;
    NtpTime time;
    char *server;
    uint16_t remotePort, localPort = 0;
    unsigned requestTime, responseTime = 0;
    unsigned long requestInterval;
    unsigned long retryInterval;
    unsigned long timeout;
};
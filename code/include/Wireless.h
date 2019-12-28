#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include <ESP8266WiFi.h>
#include <user_interface.h>

#include "BuildConfig.h"
#include "ConfigHelper.h"
#include "SysInfo.h"

#include "Utils/NetUtils.h"

typedef std::function<void(bool has, unsigned long time)> NetworkStatusChangeEventHandler;

class Wireless {
   public:
    Wireless() : out_(nullptr), mode_(NETWORK_OFF), networkStatus_(NETWORK_DOWN), ap_enabled_(false), lastUp_(0), lastDown_(0), scanning_(false){};

    void setOutput(Print *p);
    void start(const bool safe = false);
    void init(const NetworkMode mode, const char *host, const uint8_t tpw);
    bool hasNetwork();
    void setMode(const NetworkMode mode);
    void setOnStatusChange(NetworkStatusChangeEventHandler);

    void setStatus(NetworkStatus);
    void statusChangeEvent(unsigned long now = millis());

    String getModeStr();
    void refreshStatus();
    NetworkMode getMode();
                    
    void setupAP(const IPAddress host);
    bool startAP(const char *ssid, const char *password);

    void setupSTA(const IPAddress ipaddr = IP_ADDR_ANY, const IPAddress subnet = IP_ADDR_ANY,
                  const IPAddress gateway = IP_ADDR_ANY, const IPAddress dns = IP_ADDR_ANY);
    bool startSTA(const char *ssid, const char *password);
    bool disconnectWiFi();

    void setBroadcast(uint8_t);

    void startWiFiScan(bool hidden);
    bool isScanning();
    bool isActive();
    void onScanComplete(int found);
    void useStaticStationIP(bool enabled);
    
   private:
    WiFiEventHandler staGotIpEventHandler, staConnectedEventHandler, staDisconnectedEventHandler;
    WiFiEventHandler stationConnectedHandler, stationDisconnectedHandler, probeRequestHandler;
    Print *out_ = 0;    
    NetworkMode mode_;
    NetworkStatus networkStatus_;
    NetworkStatusChangeEventHandler statusChangeHandler;
    bool ap_enabled_;
    unsigned long lastUp_;
    unsigned long lastDown_;
    bool scanning_;
};
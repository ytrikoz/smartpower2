#pragma once

#include "Config.h"
#include "Core/Storage.h"

#include <ESP8266WiFi.h>

class ConfigHelper : public Printable {
  public:
    ConfigHelper(const char* name);
    ~ConfigHelper();
    size_t printTo(Print &p) const;
  public:
    void setName(const char* name);
    bool isSecured(ConfigItem i);
    bool check();
    const char* name();
    void setOutput(Print* p);
    void setDefaultConfig();
    void load();
    bool load(Config *src, Queue<String>& data);
    bool save(bool backup = false);
    bool save(Config &src, Queue<String>& data);
    bool setBootPowerState(BootPowerState);
    bool setBootPowerState(uint8_t);
    BootPowerState getBootPowerState();
    const char *getSSID();
    const char *getSSID_AP();
    const char *getPassword();
    const char *getPassword_AP();
    float getOutputVoltage();
    uint8_t getTPW();
    bool getWhStoreEnabled();
    NetworkMode getWiFiMode();    
    bool setWiFiMode(uint8_t);
    bool setWiFiMode(WiFiMode_t);
    bool setSSID(const char *);
    bool setPassword(const char *);
    bool setDHCP(bool);
    bool setIPAddress(IPAddress);
    bool setIPAddress(const char *);
    bool setNetmask(IPAddress);
    bool setNetmask(const char *);
    bool setGateway(IPAddress);
    bool setGateway(const char *);
    bool setDns(IPAddress);
    bool setDns(const char *);
    bool setOutputVoltage(float value);
    bool setNetworkSTAConfig(uint8_t, const char *, const char *, bool,
                             const char *, const char *, const char *,
                             const char *);
    bool setPowerConfig(BootPowerState state, float outputVoltage);
    bool setNtpConfig(sint8_t timeZone_h, uint16_t updateInterval_s);
    
    void setOnConfigChange(ConfigItem param);
    Config *get();
  private:
    String extractName(const String &str);
    String extractValue(const String &str);    

    Config obj_;
    char* name_;
    bool changed_;
    Print *out_;
};

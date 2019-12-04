#pragma once

#include <ESP8266WiFi.h>

#include "CommonTypes.h"
#include "Config.h"
#include "Storage.h"

class ConfigHelper : public Printable {
  public:
    size_t printTo(Print &p) const;
    ConfigHelper();
  public:
    void setDefault();
    void load();
    bool save();
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

    bool authorize(const char *login, const char *passwd);
    String getConfigJson();
    Config *get();

  private:
    String extractName(String &str);
    String extractValue(String &str);
    void loadConfig(Config *config);
    bool load(Config *config, Container<String>& data);
    bool save(Config &config, Container<String>& data);
    
    void onConfigChanged(ConfigItem param);
    
    Config *obj_;
    char name_[FILENAME_SIZE + 1];
    bool stored_;
    Print *dbg = &DEBUG;
    Print *err = &INFO;
    Print *out = &INFO;
};
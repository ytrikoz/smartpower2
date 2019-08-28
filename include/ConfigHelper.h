#pragma once

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#include "CommonTypes.h"
#include "StringQueue.h"
#include "Config.h"
#include "Cursor.h"

class ConfigHelper : public Printable {
   public:
    virtual size_t printTo(Print &p) const;
    ConfigHelper();

   public:
    void setDefault();
    void loadConfig();
    bool saveConfig();
    WiFiMode_t getWiFiMode();
    bool setBootPowerState(BootPowerState);
    bool setBootPowerState(uint8_t);
    BootPowerState getBootPowerState();
    const char *getSSID();
    const char *getSSID_AP();
    const char *getPassword();
    const char *getPassword_AP();
    bool getDHCP();
    float getOutputVoltage();
    uint8_t getTPW();
    IPAddress getIPAddr();
    IPAddress getIPAddr_AP();
    const char *getIPAddrStr();
    IPAddress getNetmask();
    const char *getNetmaskStr();
    IPAddress getGateway();
    const char *getGatewayStr();
    IPAddress getDns();
    const char *getDnsStr();
    bool getWatthHoursLogEnabled();
    
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
    void loadConfig(Config *config, StringQueue *data);
    bool saveConfig(Config *config, StringQueue *data);
    void handleParameterChanged(Parameter param);
    Config *config;
    char *filename;
    bool synced;
#ifdef DEBUG_CONFIG
    Print *dbg = &DEBUG;
#endif
    Print *err = &USE_SERIAL;
};
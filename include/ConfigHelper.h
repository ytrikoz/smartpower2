#pragma once

#include <FS.h>

#include <ArduinoJson.h>

#include "Config.h"

#include "types.h"

class ConfigHelper {
   public:
    ConfigHelper();
    ~ConfigHelper();

    void init(const char *file);
    void reload();
    void save();
    void reset();

    void setLastPowerState(PowerState state);
    PowerState getLastPowerState();

    uint8_t getWiFiMode();
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
    IPAddress getDNS();
    const char *getDNSStr();

    bool setWiFiMode(uint8_t);
    bool setSSID(const char *);
    bool setPassword(const char *);
    bool setDHCP(bool);
    bool setIPAddress(IPAddress);
    bool setIPAddress(const char *);
    bool setNetmask(IPAddress);
    bool setNetmask(const char *);
    bool setGateway(IPAddress);
    bool setGateway(const char *);
    bool setDNS(IPAddress);
    bool setDNS(const char *);
    bool setOutputVoltage(float value);

    bool setNetworkSTAConfig(uint8_t, const char *, const char *, bool,
                             const char *, const char *, const char *,
                             const char *);
    bool setPowerConfig(BootPowerState state, float outputVoltage);
    bool setNtpConfig(sint8_t timeZone_h, uint16_t updateInterval_s);

    bool authorize(const char *login, const char *passwd);

    String getConfigJson();
    Config *getData();

   private:
    void onConfigEvent(Parameter param);

    int getIntValue(Parameter param);
    bool getBoolValue(Parameter param);
    IPAddress getIPAddr(Parameter param);
    const char *getStrValue(Parameter param);
    float getFloatValue(Parameter param);

    Config *config;
    char *filename;

    bool loadFile(const char *);
    bool saveFile(const char *);

    bool synced;
#ifdef DEBUG_CONFIG
    Print *debug = &USE_DEBUG_SERIAL;
#endif
};

extern ConfigHelper *config;
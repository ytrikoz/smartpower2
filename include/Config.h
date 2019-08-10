#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include "CommonTypes.h"

typedef std::function<void(Parameter)> ConfigEventHandler;

typedef struct {
    char name[PARAM_NAME_STR_SIZE];
    size_t size;
    const char *def;
} Metadata;

class Config {
   public:
    Config();
    ~Config();

    void setDefault();
    void setOnEvents(ConfigEventHandler eventHandler);
    bool setValue(Parameter param, const char value);
    bool setValue(Parameter param, bool value);
    bool setValue(Parameter param, sint8_t value);
    bool setValue(Parameter param, uint8_t value);
    bool setValue(Parameter param, uint16_t value);
    bool setValue(Parameter param, float value);
    bool setValue(Parameter param, const char *value);
    bool setValue(const char *name, const char *value);
    void setValue(String str);

    IPAddress getIPAddrValue(Parameter param);
    const char *getStrValue(Parameter param);
    float getFloatValue(Parameter param);
    bool getBoolValue(Parameter param);
    uint8_t getByteValue(Parameter param);
    uint16_t getIntValue(Parameter param);
    sint8_t getSignedValue(Parameter param);

    bool getParameter(const char *name, Parameter &param);
    bool getParameter(const char *name, Parameter &param, size_t &size);
    void getConfigLine(Parameter param, char *str);
    const char *getName(Parameter param);
    const size_t getValueSize(Parameter param);

   private:
    void onConfigChangeEvent(Parameter param);

    ConfigEventHandler onEvents;

    char *values[PARAM_COUNT];

    Metadata metadata[PARAM_COUNT] = {
        {"wifi", PARAM_BOOL_SIZE, "2"},
        {"ssid", PARAM_STR_SIZE, "MyNetwork"},
        {"passwd", PARAM_STR_SIZE, "SomePassword"},
        {"dhcp", PARAM_BOOL_SIZE, "1"},
        {"ipaddr", PARAM_IPADDR_SIZE, "192.168.1.4"},
        {"netmask", PARAM_IPADDR_SIZE, "255.255.255.0"},
        {"gateway", PARAM_IPADDR_SIZE, "192.168.1.1"},
        {"dns", PARAM_IPADDR_SIZE, "192.168.1.1"},
        {"voltage", PARAM_OUTPUT_VOLTAGE_SIZE, "5.0"},
        {"bootpwr", PARAM_BOOL_SIZE, "0"},
        {"login", PARAM_STR_SIZE, "admin"},
        {"password", PARAM_STR_SIZE, "1234"},
        {"ap_ssid", PARAM_STR_SIZE, "SmartPower2"},
        {"ap_passwd", PARAM_STR_SIZE, "12345678"},
        {"ap_ipaddr", PARAM_IPADDR_SIZE, "192.168.4.1"},
        {"time_zone", PARAM_NUMBER_SIZE, "3"},
        {"twp", PARAM_NUMBER_SIZE, "82"},
        {"ntp_sync", PARAM_LARGE_NUMBER_SIZE, "3600"},
        {"ntp_pool", PARAM_STR_SIZE, DEF_NTP_POOL_SERVER},
        {"time_backup", PARAM_LARGE_NUMBER_SIZE, DEF_TIME_BACKUP_INTERVAL_s},
    };
};
#pragma once

#include <Arduino.h>

#include "CommonTypes.h"

typedef std::function<void(Parameter)> ParameterChangedEventHandler;

class Config {
   public:
    Config();
    ~Config();
    bool load(const char *str, size_t size);
    void setOnParameterChanged(ParameterChangedEventHandler);
    void setDefaultValue(Parameter param);
    String toString(Parameter param);
    bool setValueString(const Parameter param, const char *str);
    bool setValueStringByName(const char *name, const char *value);
    bool setValueChar(const Parameter param, char ch);
    bool setValueBool(const Parameter param, const bool value);
    bool setValueSignedByte(const Parameter param, const sint8_t value);
    bool setValueByte(const Parameter param, const uint8_t value);
    bool setValueInt(const Parameter param, const uint16_t value);
    bool setValueFloat(const Parameter param, const float value);

    Metadata getMetadata(size_t index);
    bool getValueAsBool(Parameter param);
    uint8_t getValueAsByte(Parameter param);
    sint8_t getValueAsSignedByte(Parameter param);
    uint16_t getValueAsInt(Parameter param);
    IPAddress getValueAsIPAddress(Parameter param);
    float getValueAsFloat(Parameter param);
    const char *getValueAsString(Parameter param);

    bool getParameter(const char *name, Parameter &param);
    bool getParameter(const char *name, Parameter &param, size_t &size);
    const char *getName(Parameter param);
    const size_t getSize(Parameter param);
    const char *getDefaultValue(Parameter param);

   private:
    void onChangedEvent(Parameter param);
    char *values[PARAM_COUNT];
    ParameterChangedEventHandler onParameterChanged;
    Metadata metadata[PARAM_COUNT] = {
        {"wifi", PARAM_BOOL_SIZE, "2"},
        {"ssid", PARAM_STR_SIZE, "LocalNetwork"},
        {"passwd", PARAM_STR_SIZE, "Password"},
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
        {"ntp_pool", PARAM_STR_SIZE, "pool.ntp.org"},
        {"time_backup", PARAM_LARGE_NUMBER_SIZE, "3600"},
        {"store_wh", PARAM_BOOL_SIZE, "0"}};

#ifdef DEBUG_CONFIG
    Print *dbg = &DEBUG;
#endif
};
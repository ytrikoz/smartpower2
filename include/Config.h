#pragma once

#include <Arduino.h>

#include "CommonTypes.h"

typedef std::function<void(ConfigItem)> ConfigChangeEventHandler;

class Config {
   public:
    Config();
    ~Config();
    bool load(const char *str, size_t size);
    void setOnConfigChaged(ConfigChangeEventHandler);
    void setDefaultValue(ConfigItem param);
    String toString(ConfigItem param);
    bool setValueString(const ConfigItem param, const char *str);
    bool setValueStringByName(const char *name, const char *value);
    bool setValueChar(const ConfigItem param, char ch);
    bool setValueBool(const ConfigItem param, const bool value);
    bool setValueSignedByte(const ConfigItem param, const sint8_t value);
    bool setValueByte(const ConfigItem param, const uint8_t value);
    bool setValueInt(const ConfigItem param, const uint16_t value);
    bool setValueFloat(const ConfigItem param, const float value);

    ConfigDefine getDefine(size_t index);
    const char *getName(ConfigItem param);
    const size_t getSize(ConfigItem param);
    const char *getDefaults(ConfigItem param);

    bool getValueAsBool(ConfigItem param);
    uint8_t getValueAsByte(ConfigItem param);
    sint8_t getValueAsSignedByte(ConfigItem param);
    uint16_t getValueAsInt(ConfigItem param);
    IPAddress getValueAsIPAddress(ConfigItem param);
    float getValueAsFloat(ConfigItem param);
    const char *getValueAsString(ConfigItem param);

    bool getConfig(const char *name, ConfigItem &param);
    bool getConfig(String &name, ConfigItem &param, size_t &size);
    bool getConfig(const char *name, ConfigItem &param, size_t &size);

   private:
    void onChangedEvent(ConfigItem param);
    char *values[PARAM_COUNT];
    ConfigChangeEventHandler onChangeEventHandler;

   private:
    ConfigDefine define[PARAM_COUNT] = {
        {"wifi", CONFIG_CHAR, "2"},
        {"ssid", CONFIG_STR, "LocalNetwork"},
        {"passwd", CONFIG_STR, "Password"},
        {"dhcp", CONFIG_CHAR, "1"},
        {"ipaddr", CONFIG_IPADDR, "192.168.1.4"},
        {"netmask", CONFIG_IPADDR, "255.255.255.0"},
        {"gateway", CONFIG_IPADDR, "192.168.1.1"},
        {"dns", CONFIG_IPADDR, "192.168.1.1"},
        {"voltage", CONFIG_FLOAT, "5.0"},
        {"bootpwr", CONFIG_CHAR, "0"},
        {"login", CONFIG_STR, "admin"},
        {"password", CONFIG_STR, "1234"},
        {"ap_ssid", CONFIG_STR, "SmartPower2"},
        {"ap_passwd", CONFIG_STR, "12345678"},
        {"ap_ipaddr", CONFIG_IPADDR, "192.168.4.1"},
        {"time_zone", CONFIG_NUMBER, "3"},
        {"twp", CONFIG_NUMBER, "82"},
        {"ntp_sync", CONFIG_NUMBER, "3600"},
        {"ntp_pool", CONFIG_STR, "pool.ntp.org"},
        {"time_backup", CONFIG_NUMBER, "3600"},
        {"store_wh", CONFIG_CHAR, "0"},
        {"backlight", CONFIG_CHAR, "1"}};

#ifdef DEBUG_CONFIG
    Print *dbg = &DEBUG;
#endif
};
#pragma once

#include <Arduino.h>

#include "CommonTypes.h"

typedef std::function<void(ConfigItem)> ConfigChangeEventHandler;

struct ConfigDefinition {
    const char *key_name;
    size_t value_size;
    const char *default_value;
};

class Config {
   public:
    Config();
    ~Config();
    bool load(const char *str, size_t size);
    void setOnConfigChaged(ConfigChangeEventHandler);
    void resetDefault(ConfigItem param);

    const String asString(const ConfigItem param);
    //
    bool setValueAsString(const ConfigItem param, String &str);
    bool setValueAsString(const ConfigItem param, const char *str);
    bool setValueAsStringByName(const char *name, const char *value);
    bool setValueChar(const ConfigItem param, const char ch);
    bool setValueBool(const ConfigItem param, const bool value);
    bool setValueSignedByte(const ConfigItem param, const sint8_t value);
    bool setValueByte(const ConfigItem param, const uint8_t value);
    bool setValueInt(const ConfigItem param, const uint16_t value);
    bool setValueFloat(const ConfigItem param, const float value);

    ConfigDefinition getDefinition(size_t index);
    const char *getName(ConfigItem param);
    const size_t getSize(ConfigItem param);
    const char *getDefaultValue(ConfigItem param);
    const char *getDefaultValue(uint8_t index);

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
    char *values[CONFIG_ITEMS];
    ConfigChangeEventHandler onChangeEventHandler;

   private:
    ConfigDefinition define[CONFIG_ITEMS] = {
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
        {"time_zone", CONFIG_BYTE, "18"},
        {"twp", CONFIG_BYTE, "82"},
        {"ntp_sync", CONFIG_NUMBER, "3600"},
        {"ntp_pool", CONFIG_STR, "pool.ntp.org"},
        {"time_backup", CONFIG_NUMBER, "3600"},
        {"store_wh", CONFIG_CHAR, "0"},
        {"backlight", CONFIG_CHAR, "1"},
        {"syslog", CONFIG_STR, ""}};
};
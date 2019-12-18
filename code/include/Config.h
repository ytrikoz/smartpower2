#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include "CommonTypes.h"

typedef std::function<void(ConfigItem, const char* value)> ConfigChangeEventHandler;

struct Param {
    const char *key_name;
    char *value;
    uint8_t value_size;
    const char *value_default;
};

class Config {
   public:
    Config();
    ~Config();
    void setOnChange(ConfigChangeEventHandler);
    void resetDefault(ConfigItem param);
    bool parseString(const String &str);
    String toString(const ConfigItem param) const;
    //
    bool setValueAsString(const ConfigItem param, String &str);
    bool setValue(const ConfigItem param, const char *str);
    bool setValueByName(const String &name, const String &value);
    int8_t setValueByName(const char *name, const char *value);
    bool setValueChar(const ConfigItem param, const char ch);
    bool setValueBool(const ConfigItem param, const bool value);
    bool setValueSignedByte(const ConfigItem param, const sint8_t value);
    bool setValueByte(const ConfigItem param, const uint8_t value);
    bool setValueInt(const ConfigItem param, const uint16_t value);
    bool setValueFloat(const ConfigItem param, const float value);

    bool getValueAsBool(ConfigItem param) const;
    uint8_t getValueAsByte(ConfigItem param) const;
    sint8_t getValueAsSignedByte(ConfigItem param) const;
    uint16_t getValueAsInt(ConfigItem param) const;
    IPAddress getValueAsIPAddress(ConfigItem param) const;
    float getValueAsFloat(ConfigItem param) const;
    const char *getValue(ConfigItem param) const;

    bool exist(const char *name) const;
    bool getConfig(const char *name, ConfigItem &param) const;
    bool getConfig(const String &name, ConfigItem &param, size_t &size) const;
    bool getConfig(const char *name, ConfigItem &param, size_t &size) const;

    Param getParam(size_t index) const;
    const char *getParamName(ConfigItem param) const;
    size_t getValueSize(uint8_t index) const;
    const char *getValueDefault(ConfigItem param) const;
    const char *getValueDefault(uint8_t index) const;

   private:
    void onChangedEvent(ConfigItem param);

   private:
    ConfigChangeEventHandler onChangeEventHandler;

   private:
    Param pool_[CONFIG_ITEMS] = {
        {"wifi", nullptr, CONFIG_CHAR, "2"},
        {"ssid", nullptr, CONFIG_STR, "LocalNetwork"},
        {"passwd", nullptr, CONFIG_STR, "Password"},
        {"dhcp", nullptr, CONFIG_CHAR, "1"},
        {"ipaddr", nullptr, CONFIG_IPADDR, "192.168.1.4"},
        {"netmask", nullptr, CONFIG_IPADDR, "255.255.255.0"},
        {"gateway", nullptr, CONFIG_IPADDR, "192.168.1.1"},
        {"dns", nullptr, CONFIG_IPADDR, "192.168.1.1"},
        {"voltage", nullptr, CONFIG_FLOAT, "5.0"},
        {"bootpwr", nullptr, CONFIG_CHAR, "0"},
        {"login", nullptr, CONFIG_STR, "admin"},
        {"password", nullptr, CONFIG_STR, "1234"},
        {"ap_ssid", nullptr, CONFIG_STR, "SmartPower2"},
        {"ap_passwd", nullptr, CONFIG_STR, "12345678"},
        {"ap_ipaddr", nullptr, CONFIG_IPADDR, "192.168.4.1"},
        {"time_zone", nullptr, CONFIG_BYTE, "18"},
        {"twp", nullptr, CONFIG_BYTE, "82"},
        {"ntp_sync", nullptr, CONFIG_NUMBER, "3600"},
        {"ntp_pool", nullptr, CONFIG_STR, "pool.ntp.org"},
        {"time_backup", nullptr, CONFIG_NUMBER, "3600"},
        {"store_wh", nullptr, CONFIG_CHAR, "0"},
        {"backlight", nullptr, CONFIG_CHAR, "1"},
        {"syslog", nullptr, CONFIG_STR, "192.168.1.1"}};
};
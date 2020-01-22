#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include "Config.h"
#include "Strings.h"

#define PARAMS_COUNT 24

#define PARAM_CHAR 1
#define PARAM_BYTE 3
#define PARAM_STRING 31
#define PARAM_IPADDR 16
#define PARAM_NUMBER 8
#define PARAM_FLOAT 12

struct Param {
    PGM_P key_name;
    char *value;
    uint8_t value_size;
    const char *value_default;
};

enum ConfigItem {
    WIFI,
    SSID,
    PASSWORD,
    DHCP,
    IPADDR,
    NETMASK,
    GATEWAY,
    DNS,
    OUTPUT_VOLTAGE,
    BOOT_POWER,
    LOGIN,
    PASSWD,
    AP_SSID,
    AP_PASSWORD,
    AP_IPADDR,
    TIME_ZONE,
    TPW,
    NTP_SYNC_INTERVAL,
    NTP_POOL_SERVER,
    TIME_BACKUP_INTERVAL,
    WH_STORE_ENABLED,
    BACKLIGHT,
    SYSLOG_SERVER
};

typedef std::function<void(const ConfigItem, const String &)> ConfigChangeEventHandler;

class Config {
   public:
    Config();
    ~Config();
    void setDefault(ConfigItem param);
    bool fromString(const String &str);
    String toString(const ConfigItem param) const;
    String toJsonPair(ConfigItem param);
    void setOnChange(ConfigChangeEventHandler);
    //
    int setByName(const char *name, const char *value);
    bool set(const ConfigItem param, const char *value);
    bool setChar(const ConfigItem param, const char ch);
    bool setBool(const ConfigItem param, const bool value);
    bool setSignedByte(const ConfigItem param, const sint8_t value);
    bool setByte(const ConfigItem param, const uint8_t value);
    bool setInt(const ConfigItem param, const uint16_t value);
    bool setFloat(const ConfigItem param, const float value);

    bool asBool(ConfigItem param) const;
    uint8_t asByte(ConfigItem param) const;
    sint8_t asSignedByte(ConfigItem param) const;
    uint16_t asInt(ConfigItem param) const;
    IPAddress asIPAddress(ConfigItem param) const;
    float asFloat(ConfigItem param) const;

    bool getParamByName(const char *name, ConfigItem &param) const;
    bool getParamByName(const char *name, ConfigItem &param, size_t &size) const;

    const String name(const size_t index) const;
    const char *value(const size_t index) const;
    size_t getValueSize(size_t index) const;
    const char *getDefault(const size_t index) const;
    bool exist(const char *name) const;

   private:
    const Param *get(const size_t index) const;

    void onChangedEvent(ConfigItem param);

   private:
    ConfigChangeEventHandler onChangeEventHandler;

    Param pool_[PARAMS_COUNT] = {
        {str_wifi, nullptr, PARAM_CHAR, "2"},
        {str_ssid, nullptr, PARAM_STRING, "LocalNetwork"},
        {str_passwd, nullptr, PARAM_STRING, "Password"},
        {str_dhcp, nullptr, PARAM_CHAR, "1"},
        {str_ipaddr, nullptr, PARAM_IPADDR, "192.168.1.4"},
        {str_netmask, nullptr, PARAM_IPADDR, "255.255.255.0"},
        {str_gateway, nullptr, PARAM_IPADDR, "192.168.1.1"},
        {str_dns, nullptr, PARAM_IPADDR, "192.168.1.1"},
        {str_voltage, nullptr, PARAM_FLOAT, "5.0"},
        {str_bootpwr, nullptr, PARAM_CHAR, "0"},
        {str_login, nullptr, PARAM_STRING, "admin"},
        {str_password, nullptr, PARAM_STRING, "1234"},
        {str_ap_ssid, nullptr, PARAM_STRING, "SmartPower2"},
        {str_ap_passwd, nullptr, PARAM_STRING, "12345678"},
        {str_ap_ipaddr, nullptr, PARAM_IPADDR, "192.168.4.1"},
        {str_time_zone, nullptr, PARAM_BYTE, "18"},
        {str_twp, nullptr, PARAM_BYTE, "82"},
        {str_ntp_sync, nullptr, PARAM_NUMBER, "3600"},
        {str_ntp_pool, nullptr, PARAM_STRING, "pool.ntp.org"},
        {str_time_backup, nullptr, PARAM_NUMBER, "3600"},
        {str_store_wh, nullptr, PARAM_CHAR, "0"},
        {str_backlight, nullptr, PARAM_CHAR, "1"},
        {str_syslog, nullptr, PARAM_STRING, "192.168.1.1"},
        {str_domain, nullptr, PARAM_STRING, ""}};
};
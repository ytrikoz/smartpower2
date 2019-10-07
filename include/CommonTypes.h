#pragma once

#include <Arduino.h>
#include <time.h>

#include "Consts.h"
#include "StrUtils.h"
#include "Strings.h"
#include "TimeUtils.h"

typedef std::function<void(unsigned long epoch_s)> TimeEventHandler;

enum AppModuleEnum {
    MOD_BTN,
    MOD_CLOCK,
    MOD_HTTP,
    MOD_DISPLAY,
    MOD_LED,
    MOD_NETSVC,
    MOD_NTP,
    MOD_PSU,
    MOD_SHELL,
    MOD_TELNET,
    MOD_UPDATE
};

enum StoreState { SS_UNSET, SS_CLOSED, SS_READING, SS_WRITING, SS_EOF };

enum StoreError {
    SE_OK,
    SE_INVALID,
    SE_NOT_EXIST,
    SE_ERROR_CLOSE,
    SE_ERROR_READ,
    SE_ERROR_WRITE,
};

struct PsuInfo : Printable {
    unsigned long time;
    float V;
    float I;
    float P;
    double mWh;

  public:
    PsuInfo() { time = V = I = P = mWh = 0; }

    PsuInfo(unsigned long time_ms, float V, float I, float P, double mWh)
        : time(time_ms), V(V), I(I), P(P), mWh(mWh){};

    String toString() const {
        String res = "";
        res += String(V, 3);
        res += "V, ";
        res += String(I, 3);
        res += "A, ";
        res += String(P, 3);
        res += "W, ";
        res += String(mWh / ONE_WATT_mW, 3);
        res += "Wh";
        return res;
    }

    size_t printTo(Print &p) const {
        size_t n = 0;
        n += p.print(V, 3);
        n += p.print("V, ");
        n += p.print(I, 3);
        n += p.print("A, ");
        n += p.print(P, 3);
        n += p.print("W, ");
        n += p.print(mWh / ONE_WATT_mW, 3);
        n += p.print("Wh");
        return n;
    }
};

class Logger {
  public:
    virtual PsuInfo getInfo() = 0;
};

enum BootPowerState {
    BOOT_POWER_OFF = 0,
    BOOT_POWER_ON = 1,
    BOOT_POWER_LAST_STATE = 2
};

enum EOLType { CRLF, LFCR, LF, CR };

enum MoveDirection { MD_LEFT, MD_RIGHT, MD_UP, MD_DOWN };

enum State { ST_INACTIVE, ST_NORMAL, ST_ESC_SEQ, ST_CTRL_SEQ };

struct ConfigDefine {
    const char *key_name;
    size_t value_size;
    const char *defaults;
};

#define PARAM_COUNT 22

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
    POWER,
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
    BACKLIGHT
};

struct WebClient {
    bool connected = false;
    uint8_t page = 0;
};

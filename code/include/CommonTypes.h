#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <time.h>

#include "Consts.h"
#include "Strings.h"
#include "Core/Error.h"

#include "Utils/TimeUtils.h"

enum NetworkMode {
    NETWORK_OFF,
    NETWORK_STA,
    NETWORK_AP,
    NETWORK_AP_STA
};

enum NetworkStatus { NETWORK_DOWN = 0,
                     NETWORK_UP = 1 };

struct AppLogItem {
    String str;
};

enum PsuLogEnum {
    VOLTAGE = 0,
    CURRENT = 1,
};

enum LogLevel { LEVEL_ERROR,
                LEVEL_WARN,
                LEVEL_INFO };

enum PsuState { POWER_ON = 0,
                POWER_OFF = 1 };

enum PsuStatus { PSU_OK,
                 PSU_ERROR,
                 PSU_ALERT };

enum PsuError { PSU_ERROR_NONE,
                PSU_ERROR_LOW_VOLTAGE };

enum PsuAlert { PSU_ALERT_NONE,
                PSU_ALERT_LOAD_LOW };

enum StoreState { SS_UNSET,
                  SS_CLOSED,
                  SS_READING,
                  SS_WRITING,
                  SS_EOF };

enum StoreError {
    SE_NONE = 0,
    SE_INVALID,
    SE_NOT_EXIST,
    SE_ERROR_CLOSE,
    SE_ERROR_READ,
    SE_ERROR_WRITE,
};

struct PsuData : Printable {
    unsigned long time;
    float V;
    float I;
    float P;
    double Wh;

   public:
    PsuData() { time = V = I = P = Wh = 0; }

    PsuData(unsigned long time_ms, float V, float I, float P, double Wh)
        : time(time_ms), V(V), I(I), P(P), Wh(Wh){};

    void reset(void) { time = V = I = P = Wh = 0; }

    String toString() const {
        String res = "";
        res += String(V, 3);
        res += "V, ";
        res += String(I, 3);
        res += "A, ";
        res += String(P, 3);
        res += "W, ";
        res += String(Wh, 3);
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
        n += p.print(Wh, 3);
        n += p.print("Wh");
        return n;
    }
};

class PsuListener {
   public:
    virtual void onPsuData(PsuData &item){};
};

typedef std::function<void(PsuState, PsuStatus)> PsuStateChangeHandler;

typedef std::function<void(PsuData)> PsuDataHandler;

enum BootPowerState {
    BOOT_POWER_OFF = 0,
    BOOT_POWER_ON = 1,
    BOOT_POWER_LAST_STATE = 2
};

enum EOLType { CRLF,
               LFCR,
               LF,
               CR };

enum MoveDirection { MD_LEFT,
                     MD_RIGHT,
                     MD_UP,
                     MD_DOWN };

enum State { ST_INACTIVE,
             ST_NORMAL,
             ST_ESC_SEQ,
             ST_CTRL_SEQ };

struct WebClient {
    bool connected = false;
    uint8_t page = 0;
};

#define CONFIG_ITEMS 23
#define APP_MODULES 11

enum ModuleEnum {
    MOD_BTN,
    MOD_CLOCK,
    MOD_WEB,
    MOD_DISPLAY,
    MOD_LED,
    MOD_NETSVC,
    MOD_PSU,
    MOD_SHELL,
    MOD_TELNET,
    MOD_UPDATE,
    MOD_SYSLOG
};

enum ModuleState {
    STATE_INIT,
    STATE_INIT_FAILED,
    STATE_INIT_COMPLETE,
    STATE_START_FAILED,
    STATE_ACTIVE
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
    BACKLIGHT,
    SYSLOG_SERVER
};

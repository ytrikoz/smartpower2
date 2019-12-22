#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <time.h>

#include "Consts.h"
#include "Strings.h"
#include "Core/Error.h"

#include "Utils/TimeUtils.h"

enum WebPageEnum {
    PAGE_HOME = 1,
    PAGE_OPTIONS = 2,
    PAGE_INFO = 3
};

enum AppState {
    STATE_NORMAL,
    STATE_RESTART,
    STATE_RESET
};

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

enum PsuState { POWER_OFF,
                POWER_ON };

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
        res += "I, ";
        res += String(P, 3);
        res += "P, ";
        res += String(Wh, 3);
        res += "Wh";
        return res;
    }

    String toJson() const {
        String res = "{";
        res += "\"V\":" + String(V, 3) + ",";
        res += "\"I\":" + String(I, 3) + ",";
        res += "\"P\":" + String(P, 3) + ",";
        res += "\"Wh\":" + String(Wh, 3) + "}";
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

class PsuDataListener {
   public:
    virtual void onPsuData(PsuData &item){};
};

typedef std::function<void(PsuState)> PsuStateChangeHandler;
typedef std::function<void(PsuStatus)> PsuStatusChangeHandler;

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

#define CONFIG_ITEMS 23
#define APP_MODULES 11

enum ModuleEnum {
    MOD_LED,
    MOD_BTN,
    MOD_CLOCK,
    MOD_PSU,
    MOD_DISPLAY,
    MOD_CONSOLE,
    MOD_NETSVC,
    MOD_TELNET,
    MOD_UPDATE,
    MOD_SYSLOG,
    MOD_WEB
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
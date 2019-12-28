#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <time.h>
#include <IPAddress.h>

#include "Consts.h"
#include "Strings.h"
#include "Core/Error.h"

#include "Utils/TimeUtils.h"

enum WebPageEnum {
    PAGE_HOME = 1,
    PAGE_OPTIONS = 2,
    PAGE_INFO = 3
};

enum BootPowerState {
    BOOT_POWER_OFF = 0,
    BOOT_POWER_ON = 1,
    BOOT_POWER_LAST_STATE = 2
};

enum NetworkMode {
    NETWORK_OFF,
    NETWORK_STA,
    NETWORK_AP,
    NETWORK_AP_STA
};

enum NetworkStatus { NETWORK_DOWN = 0,
                     NETWORK_UP = 1 };

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

    char* prettyNumber(char* buf, float value, const char* name) const {
        char tmp[8];
        if (I < 0.5) {
            strcpy(buf, dtostrf(value * 1000, 3, 0, tmp));
            strcat(buf, "m");
        } else {
            strcpy(buf, dtostrf(value, 2, 2, tmp));
        }
        strcat(buf, name);
        return buf;
    }

    size_t toPretty(char* buf) const {
        char tmp[32];
        memset(tmp, 0, 32);
        // Volt
        strcat(buf, dtostrf(V, 2, 2, tmp));
        strcat(buf, "V ");
        // Amper
        strcat(buf, prettyNumber(tmp, I, "A "));
        // Watt
        strcat(buf, dtostrf(P, 2, 2, tmp));
        strcat(buf, "W");

        return strlen(buf);
    }

    String toJson() const {
        String res = "{";
        res += "\"v\":" + String(V, 3) + ",";
        res += "\"i\":" + String(I, 3) + ",";
        res += "\"p\":" + String(P, 3) + ",";
        res += "\"wh\":" + String(Wh, 6) + "}";
        return res;
    }

    size_t printTo(Print& p) const {
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
    virtual void onPsuData(PsuData& item){};
};

struct NetInfo {
    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;
    NetInfo(){};
    NetInfo(ip_info& info) {
        ip = IPAddress(info.ip.addr);
        subnet = IPAddress(info.netmask.addr);
        gateway = IPAddress(info.gw.addr);
    }
};


enum EOLType { CRLF,
               LFCR,
               LF,
               CR };

enum MoveDirection { MD_LEFT,
                     MD_RIGHT,
                     MD_UP,
                     MD_DOWN };


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

#define PARAMS_COUNT 23
#define MODULES_COUNT 11
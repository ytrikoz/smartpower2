#pragma once
#include <Arduino.h>

#include "consts.h"

typedef struct DateTime : public Printable {
    unsigned long epochTime_s = 0;

    size_t printTo(Print& p) const {
        size_t res = p.printf("epoch %lu", epochTime_s);
        return res;
    }
} DateTime;

typedef struct Measurement : public Printable {
    unsigned long updated_ms;
    float voltage;
    float current;
    float power;
    double wattSeconds;
    size_t printTo(Print& p) const {
        char buf[32];
        size_t size = sprintf(buf, "%.3f,%.3f,%.3f,%.3f", voltage, current,
                              power, wattSeconds * ONE_HOUR_s);
        p.println(buf);
        return size;
    }
} Measurement;

typedef enum {
    BOOT_POWER_OFF = 0,
    BOOT_POWER_ON = 1,
    BOOT_POWER_LAST_STATE = 2
} BootPowerState;

typedef enum { POWER_ON = 0, POWER_OFF = 1 } PowerState;

typedef enum { CRLF, LFCR, LF, CR } EOLCode;

typedef enum { MD_LEFT, MD_RIGHT, MD_UP, MD_DOWN } MoveDirection;

typedef enum { ST_INACTIVE, ST_NORMAL, ST_ESC_SEQ, ST_CTRL_SEQ } State;

typedef enum {
    WLAN_OFF = 0,
    WLAN_STA = 1,
    WLAN_AP = 2,
    WLAN_AP_STA = 3
} WirelessMode;

typedef enum { NETWORK_DOWN, NETWORK_UP } NetworkState;

typedef enum {
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
    TIME_BACKUP_INTERVAL
} Parameter;
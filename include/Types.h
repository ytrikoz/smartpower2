#pragma once

#include <Arduino.h>

#include "consts.h"

typedef struct {
    char name[4];
    uint8_t days;
    uint16_t dayOfyear;
} Month;

static const Month calendar[] = {
    {"Jan", 31, 0},   {"Feb", 28, 31},  {"Mar", 31, 59},  {"Apr", 30, 90},
    {"May", 31, 120}, {"Jun", 30, 151}, {"Jul", 31, 181}, {"Aug", 31, 212},
    {"Sep", 30, 243}, {"Oct", 31, 273}, {"Nov", 30, 304}, {"Dec", 31, 334}};

typedef struct EpochTime : public Printable {
   public:
    EpochTime() {
        this->epoch_s = 0;
        this->wasSet = false;
    }
    EpochTime(unsigned long epoch_s) {
        this->epoch_s = epoch_s;
        wasSet = true;
    }

    unsigned long get() { return epoch_s; }

    size_t printTo(Print& p) const {
        size_t n;
        if (wasSet)
            n = p.printf_P(strf_epoch, epoch_s);
        else
            n = p.printf_P(str_unset);
        return n;
    }

   private:
    unsigned long epoch_s;
    bool wasSet;

} EpochTime;

typedef struct : public Printable {
   public:
    void Time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
        this->hour = hour;
        this->minute = minute;
        this->seconds = seconds;
    }
    size_t printTo(Print& p) const {
        size_t n = p.printf("%02d:%02d:%02d", hour, minute, seconds);
        return n;
    }
    unsigned long get() {
        return hour * ONE_HOUR_s + minute * ONE_MINUTE_s + seconds;
    }

   private:
    uint8_t hour = 0;
    uint8_t minute = 0;
    uint8_t seconds = 0;
} Time;

typedef struct Date : public Printable {
   public:
    Date(struct tm& tm) {
        this->day = tm.tm_mday;
        this->month = tm.tm_mon;
        this->month = tm.tm_year;
    }

    Date(uint8_t day, uint8_t month, int year) {
        this->day = day;
        this->month = month;
        this->year = year;
    }
    unsigned long get() { return 0; }

    size_t printTo(Print& p) const {
        size_t n = p.printf("%02d.%02d.%04d", day, month, year);
        return n;
    }

   private:
    uint8_t day = 0;
    uint8_t month = 0;
    uint16_t year = 0;
} Date;

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
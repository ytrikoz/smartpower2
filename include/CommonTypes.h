#pragma once

#include <Print.h>
#include <time.h>

#include "Strings.h"

typedef struct {
    char name[4];
    uint8_t days;
    uint16_t dayOfyear;
} Month;

struct EpochTime : public Printable {
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
};

struct Time : public Printable {
   public:
    Time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
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
};

struct Date : public Printable {
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
        size_t n = p.printf("%02d/%02d/%4d", day, month, year);
        return n;
    }

   private:
    uint8_t day = 0;
    uint8_t month = 0;
    uint16_t year = 0;
};

struct PsuInfo {
    unsigned long time;
    float voltage;
    float current;
    float power;
    double wattSeconds;

   public:
    PsuInfo() {
        time = 0;
        voltage = 0;
        current = 0;
        power = 0;
        wattSeconds = 0;
    }
    PsuInfo(unsigned long time_ms, float voltage, float current, float power,
            double wattSeconds)
        : time(time_ms),
          voltage(voltage),
          current(current),
          power(power),
          wattSeconds(wattSeconds){};
};

class PsuInfoProvider {
   public:
    virtual PsuInfo getInfo();
};

enum BootPowerState {
    BOOT_POWER_OFF = 0,
    BOOT_POWER_ON = 1,
    BOOT_POWER_LAST_STATE = 2
};

enum PowerState { POWER_ON = 0, POWER_OFF = 1 };

enum EOLCode { CRLF, LFCR, LF, CR };

enum MoveDirection { MD_LEFT, MD_RIGHT, MD_UP, MD_DOWN };

enum State { ST_INACTIVE, ST_NORMAL, ST_ESC_SEQ, ST_CTRL_SEQ };

enum WirelessMode { WLAN_OFF = 0, WLAN_STA = 1, WLAN_AP = 2, WLAN_AP_STA = 3 };

enum NetworkState { NETWORK_DOWN, NETWORK_UP };

enum Parameter {
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
};

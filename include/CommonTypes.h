#pragma once

#include <Arduino.h>
#include <time.h>

#include "Consts.h"
#include "StrUtils.h"
#include "Strings.h"
#include "TimeUtils.h"

enum StoreState { SS_UNSET, SS_CLOSED, SS_READING, SS_WRITING, SS_EOF };

enum StoreError {
    SE_OK,
    SE_INVALID,
    SE_NOT_EXIST,
    SE_ERROR_CLOSE,
    SE_ERROR_READ,
    SE_ERROR_WRITE,
};

enum AppModuleEnum {
    MOD_BTN,
    MOD_CLOCK,
    MOD_LED,
    MOD_PSU,
    MOD_TASK,
    MOD_TELNET_SHELL,
    MOD_LCD,
    MOD_HTTP,
    MOD_NETSVC,
    MOD_NTP,
    MOD_TELNET,
    MOD_SERIAL_SHELL,
    MOD_UPDATE
};

struct EpochTime : public Printable {
   public:
    size_t printTo(Print& p) const { return p.print(epoch_s); }
    uint8_t n = sizeof(EpochTime);

   public:
    EpochTime() : EpochTime(0) {}
    EpochTime(unsigned long epoch_s) { this->epoch_s = epoch_s; }
    void tick() { this->epoch_s++; };
    unsigned long toEpoch() { return this->epoch_s; }
    String toString() { return String(this->epoch_s); }
    uint8_t getDayOfWeek(unsigned long epoch_s) {
        return (((epoch_s / ONE_DAY_s) + 4) % ONE_WEEK_days);
    }
    uint8_t asTimeHour(unsigned long epoch_s) {
        return (epoch_s % ONE_DAY_s) / ONE_HOUR_s;
    }
    uint8_t asTimeMinute(unsigned long epoch_s) {
        return (epoch_s % ONE_HOUR_s) / ONE_MINUTE_s;
    }
    uint8_t asTimeSecond(unsigned long epoch_s) {
        return epoch_s % ONE_MINUTE_s;
    }

   private:
    unsigned long epoch_s;
};

struct Time : EpochTime {
   public:
    Time() : Time(0, 0, 0) {}
    Time(struct tm& tm) : Time(tm.tm_hour, tm.tm_min, tm.tm_sec) {}
    Time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
        this->hour = hour;
        this->minute = minute;
        this->seconds = seconds;
    }
    String timeAsString() {
        char buf[64];
        sprintf(buf, "%02d:%02d:%02d", hour, minute, seconds);
        return String(buf);
    }
    unsigned long asEpoch() {
        return hour * ONE_HOUR_s + minute * ONE_MINUTE_s + seconds;
    }

   protected:
    uint8_t hour;
    uint8_t minute;
    uint8_t seconds;
};

struct DateTime : Time {
   public:
    DateTime(struct tm& tm)
        : DateTime(tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour, tm.tm_min,
                   (uint8_t)tm.tm_sec) {}

    DateTime(uint16_t year, uint8_t month, uint8_t mday, uint8_t hour,
             uint8_t minute, uint8_t second) {
        this->mday = mday;
        this->month = month;
        this->year = year;
    }

    unsigned long asEpoch() {
        return (this->year - START_YEAR) * ONE_YEAR_days +
               ((this->year - START_YEAR - 1) / 4) +
               isLeapYear(this->year) * ONE_DAY_s +
               getDaysInMonth(this->month, this->year) + this->mday - 1;
    }

    String asString() {
        char buf[64];
        sprintf(buf, "%02d:%02d:%04d ", mday, month, year);
        sprintf(buf, timeAsString().c_str());
        return String(buf);
    }

   private:
    uint8_t mday;
    uint8_t month;
    uint16_t year;
};

struct Month {
    const char* name;
    uint8_t mdays;
    uint16_t yday;
    Month(const char* name, uint8_t mdays, uint16_t yday) {
        this->name = name;
        this->mdays = mdays;
        this->yday = yday;
    }
    unsigned long toEpoch() { return this->mdays * ONE_DAY_s; }
};

enum PsuLogItem { VOLTAGE_LOG, CURRENT_LOG, POWER_LOG, WATTSHOURS_LOG };

struct LogItem {
    size_t n;
    uint16_t value;
    LogItem() : n(0), value(0){};
    LogItem(size_t n, uint16_t value) : n(n), value(value){};
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

    size_t printTo(Print& p) const {
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

class PsuInfoProvider {
   public:
    virtual PsuInfo getInfo() = 0;
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

typedef struct {
    const char* name;
    size_t size;
    const char* default_value;
} Metadata;

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
    TIME_BACKUP_INTERVAL,
    WH_STORE_ENABLED
} Parameter;

typedef struct {
    bool connected = false;
    uint8_t page = 0;
} WebClient;

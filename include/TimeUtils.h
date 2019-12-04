#pragma once

#include <Arduino.h>
#include <time.h>

#include "Consts.h"

unsigned long millis_since(unsigned long since);

unsigned long millis_passed(unsigned long start, unsigned long finish);

struct EpochTime : public Printable {
   public:
    size_t printTo(Print &p) const { return p.print(epoch_s); }

   public:
    EpochTime() : EpochTime(0) {}

    EpochTime(unsigned long epoch_s) { this->epoch_s = epoch_s; }

    void tick() { this->epoch_s++; };

    unsigned long toEpoch() { return this->epoch_s; }

    String toString() { return String(this->epoch_s); }

    uint8_t asHours(unsigned long s) { return s / ONE_HOUR_s; }

    uint8_t asMinutes(unsigned long s) { return s / ONE_MINUTE_s; }

   private:
    unsigned long epoch_s;
};

struct Date {
   public:
    Date(uint16_t year, uint8_t month, uint8_t day) {
        this->year = year;
        this->month = month;
        this->day = day;
    }

    Date(struct tm &tm) : Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday) {}

    String dateFormated() {
        char buf[16];
        sprintf(buf, "%02d:%02d:%04d ", day, month, year);
        return String(buf);
    }

    unsigned long asEpoch() {
        struct tm t;
        // Year - 1900
        t.tm_year = this->year - 1900;
        // Month, where 0 = jan
        t.tm_mon = this->month;
        // Day of the month
        t.tm_mday = this->day;
        // Time 24 hours
        t.tm_hour = t.tm_min = t.tm_sec = 0;
        // Is DST on? 1 = yes, 0 = no, -1 = unknown
        t.tm_isdst = -1;
        return mktime(&t);
    }

   protected:
    uint16_t year;
    uint8_t day;
    uint8_t month;
};

struct DateTime : public Date, public Printable {
   public:
    size_t printTo(Print &p) const {
        char buf[32];
        sprintf_P(buf, DATETIME_FORMAT, day, month, year, hour, minute,
                  seconds);
        return p.print(buf);
    }

   protected:
    uint8_t hour;
    uint8_t minute;
    uint8_t seconds;

   public:
    DateTime(struct tm &tm) : Date(tm) {
        this->hour = tm.tm_hour;
        this->minute = tm.tm_min;
        this->seconds = tm.tm_sec;
    }

    DateTime(uint16_t year, uint8_t month, uint8_t day, uint8_t hour,
             uint8_t minute, uint8_t second)
        : Date(year, month, day) {
        this->hour = hour;
        this->minute = minute;
        this->seconds = second;
    }

    void setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
        this->hour = hour;
        this->minute = minute;
        this->seconds = seconds;
    }

    unsigned long asEpoch() {
        struct tm t;
        // Year - 1900
        t.tm_year = this->year - 1900;
        // Month, where 0 = jan
        t.tm_mon = this->month - 1;
        // Day of the month
        t.tm_mday = this->day;
        // Time 24 hours
        t.tm_hour = this->hour;
        t.tm_min = this->minute;
        t.tm_sec = this->seconds;
        // Is DST on? 1 = yes, 0 = no, -1 = unknown
        t.tm_isdst = -1;
        return mktime(&t);
    }

    String timeFormated() {
        char buf[16];
        sprintf_P(buf, TIME_FORMAT, hour, minute, seconds);
        return String(buf);
    }

    String dateTimeFormated() {
        char buf[64];
        sprintf_P(buf, DATETIME_FORMAT, day, month, year, hour, minute,
                  seconds);
        return String(buf);
    }
};

namespace TimeUtils {

uint16_t daysInYear(uint16_t year);

uint8_t daysInMonth(uint8_t month, uint16_t year);

bool isLeapYear(uint16_t year);

uint16_t encodeYear(uint16_t &year);

bool encodeMonth(String str, uint8_t &month);

bool encodeDate(const char *str, struct tm &tm);

bool encodeTime(const char *str, struct tm &tm);

bool isValidMonth(uint8_t month);

bool isValidHour(uint8_t hour);

bool isValidMinute(uint8_t minute);

bool isValidSecond(uint8_t second);

unsigned long encodeEpoch(tm tm);

unsigned long encodeEpochTime(uint8_t hour, uint8_t minute, uint8_t second);

unsigned long encodeEpochDate(uint16_t year, uint8_t month, uint16_t day);

size_t tmtodtf(struct tm &tm, char *str);

int timeZoneInSeconds(const byte timeZone);

int timeZoneInHours(const byte timeZone);

void format_elapsed_time(char *buf, time_t elapsed);

void format_elapsed_time(char *buf, double elapsed);
}  // namespace TimeUtils


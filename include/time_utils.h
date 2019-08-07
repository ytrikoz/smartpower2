#pragma once

#include <pgmspace.h>

#include "Types.h"
#include "time.h"

// #define DEBUG_TIME_UTILS

static const char stre_invalid_date[] PROGMEM =
    "%d/%d/%d - is not a valid date\r\n";
static const char stre_invalid_time[] PROGMEM =
    "%2d:%2d:%2d - is not a valid time\r\n";

uint8_t get_days_in_month(uint8_t month, uint16_t year);
uint8_t str_to_month(String str);

bool is_leap_year(uint16_t year);
bool is_valid_date(int day, int month, int year);
bool is_valid_year(uint16_t n);
bool is_valid_month(uint8_t n);
bool is_valid_hour(uint8_t n);
bool is_valid_minute(uint8_t n);
bool is_valid_seconds(uint8_t n);
bool is_valid_day(uint8_t n);

bool str_to_date(char *str, struct tm &tm);
bool str_to_time(char *str, struct tm &tm);

 long millis_since(unsigned long since);
 long millis_passed(unsigned long start, unsigned long finish);

void epoch_to_tm(unsigned long epoch_s, struct tm &tm);

unsigned long get_appbuild_epoch();
uint32_t get_epoch(tm dateTime);
uint32_t get_epoch(int year, int month, int day, int hour, int minute,
                   int second);
inline char *tmtoa(struct tm *tm, char *buf) {
    sprintf(buf, DATETIME_FORMAT, tm->tm_mday, tm->tm_mon, tm->tm_year,
            tm->tm_hour, tm->tm_min, tm->tm_sec);
    return buf;
}

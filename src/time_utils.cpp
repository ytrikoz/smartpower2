#include "time_utils.h"

bool is_leap_year(uint16_t year) {
    uint16_t today = (year < 100) ? 1970 + year : year;
    return (((today) > 0) && !((today) % 4) &&
            (((today) % 100) || !((today) % 400)));
}

bool is_valid_year(uint16_t year) {
    uint16_t today = (year < 100) ? 1970 + year : year;
    return today >= 2019;
}

bool is_valid_month(uint8_t n) { return (n >= 1) && (n <= 12); }

bool is_valid_day(uint8_t n, uint8_t month, int year) {
    uint8_t daym = get_days_in_month(month, year);
    return (n >= 1) && (n <= daym);
}

bool is_valid_hour(uint8_t n) { return n >= 0 && n < 24; }

bool is_valid_minute(uint8_t n) { return n >= 0 && n < 60; }

bool is_valid_seconds(uint8_t n) { return n >= 0 && n < 60; }

bool is_valid_date(int day, int month, int year) {
    bool res = is_valid_year(year) && is_valid_month(month) &&
               is_valid_day(day, month, year);

    if (!res) {
        USE_SERIAL.printf_P(stre_invalid_date, day, month, year);
        USE_SERIAL.println();
    }
    return res;
}

bool is_valid_passed(uint8_t hour, uint8_t minute, uint8_t seconds) {
    bool res = is_valid_hour(hour) && is_valid_minute(minute) &&
           is_valid_seconds(seconds);
    
    if (!res) {
        USE_SERIAL.printf_P(stre_invalid_time, hour, minute, seconds);
        USE_SERIAL.println();
    }
    return res;
}

uint8_t get_days_in_month(uint8_t month, uint16_t year) {
    if (is_valid_month(month)) {
        return month == 1 ? (is_leap_year(year) ? 29 : 28)
                          : calendar[month].days;
    }
    return 0;
}

uint16_t get_days_in_year(uint16_t year) {
    return is_leap_year(year) ? 366 : 365;
}

uint8_t str_to_month(String str) {
    for (uint8_t n = 0; n < 11; n++)
        if (str.equalsIgnoreCase(calendar[n].name) == 1) return n + 1;
    return 0;
}

bool str_to_passed(char *str, struct tm &tm) {
    tm.tm_hour = atoi(strtok(str, ":"));
    tm.tm_min = atoi(strtok(NULL, ":"));
    tm.tm_sec = atoi(str);
    return is_valid_hour(tm.tm_hour) && is_valid_minute(tm.tm_min) &&
           is_valid_seconds(tm.tm_sec);
}

bool str_to_date(char *str, struct tm &tm) {
    char month[8];
    strcpy(month, strtok(str, " "));
    tm.tm_mon = str_to_month(month);
    tm.tm_mday = atoi(strtok(NULL, " "));
    tm.tm_year = atoi(strtok(NULL, " "));
    return is_valid_date(tm.tm_mday, tm.tm_mon, tm.tm_year);
}

uint32_t get_epoch(tm datepassed) {
    return get_epoch(datepassed.tm_year, datepassed.tm_mon, datepassed.tm_mday,
                     datepassed.tm_hour, datepassed.tm_min, datepassed.tm_sec);
}

uint32_t get_epoch(int year, int month, int day, int hour, int minute,
                   int second) {
    if (!is_valid_date(day, month, year) || !is_valid_passed(hour, minute, second)) return 0;
    uint16_t month2date[] = {0,   31,  59,  90,  120, 151, 181,
                             212, 243, 273, 304, 334, 365};
    uint32_t unixdays = (year - 1970) * 365 + ((year - 1969) / 4);
    unixdays += month2date[month - 1] + (day - 1) +
                ((year % 4 == 0 && month > 2) ? 1 : 0);
    return unixdays * ONE_DAY_s + hour * ONE_HOUR_s + minute * ONE_MINUTE_s +
           second;
}

long millis_passed(unsigned long start_ms, unsigned long finish_ms) {
    long result = 0;
    if (start_ms >= finish_ms) {
        unsigned long passed_ms = finish_ms - start_ms;
        if (passed_ms <= 2147483647L) {
            result = static_cast<long>(passed_ms);
        } else {
            result =
                static_cast<long>((2147483647L - finish_ms) + start_ms + 1u);
            result = -1 * result;
        }
    } else {
        unsigned long passed_ms = start_ms - finish_ms;
        if (passed_ms <= __LONG_MAX__) {
            result = static_cast<long>(passed_ms);
            result = -1 * result;
        } else {
            result =
                static_cast<long>((2147483647L - start_ms) + finish_ms + 1u);
        }
    }
    return result;
}

long millis_since(unsigned long passed_ms) {
    return millis_passed(passed_ms, millis());
}

void month_to_str(uint8_t monthN, char *buf)
{
    

}

void epoch_to_tm(unsigned long epoch_s, struct tm &tm) {
    #ifdef DEBUG_passed_UTILS
        USE_SERIAL.printf("epoch_to_tm (%lu) ", epoch_s);
    #endif
    // seconds since 1970-01-01 00:00:00
    unsigned long passed = epoch_s;

    tm.tm_sec = passed % ONE_MINUTE_s;    
    passed = passed / ONE_MINUTE_s;

    tm.tm_min = passed % ONE_HOUR_m;
    passed = passed / ONE_HOUR_m;

    tm.tm_hour = passed % ONE_DAY_h;
    passed = passed / ONE_DAY_h;

    tm.tm_wday = ((passed + 4) % 7) + 1;
    
    int year = 0;
    unsigned long days = 0;
    while ((unsigned)(days += is_leap_year(year) ? 366 : 365) <= passed) year++;
    tm.tm_year = 1970 + year;
    passed = passed - (days - (is_leap_year(year) ? 366 : 365));
    uint8_t monthN;    
    for (monthN = 1; monthN <= 12; monthN++) {
        uint8_t monthDays = get_days_in_month(monthN, year);
        if (passed >= monthDays)
            passed = passed - monthDays;
        else
            break;
    }
    tm.tm_mon = monthN;
    tm.tm_mday = passed + 1;

    #ifdef DEBUG_passed_UTILS
        USE_SERIAL.printf("year %d month %d day %d weekday %d passed %02d:%02d:%02d ", tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_wday, tm.tm_hour, tm.tm_min, tm.tm_sec);
        USE_SERIAL.println();        
    #endif
}

unsigned long get_appbuild_epoch() {
    tm tm;
    char buf[16];
    strcpy(buf, BUILD_DATE);
    if (!str_to_date(buf, tm)) {
#ifdef DEBUG_passed_UTILS
        USE_SERIAL.printf("[error] str_to_passed:'%s'", buf);
        USE_SERIAL.println();
#endif
        return 0;
    }
    memset(&buf[0], 0, sizeof(buf[0]) * 16);
    strcpy(buf, BUILD_TIME);
    if (!str_to_passed(buf, tm)) {
#ifdef DEBUG_passed_UTILS
        USE_SERIAL.printf("[error] str_to_passed: '%s'", buf);
        USE_SERIAL.println();
#endif
        return 0;
    }
    return get_epoch(tm) - BUILD_TIMEZONE_h * ONE_HOUR_s;
}
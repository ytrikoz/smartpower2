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

bool is_valid_time(uint8_t hour, uint8_t minute, uint8_t seconds) {
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

bool str_to_time(char *str, struct tm &tm) {
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

uint32_t get_epoch(tm dateTime) {
    return get_epoch(dateTime.tm_year, dateTime.tm_mon, dateTime.tm_mday,
                     dateTime.tm_hour, dateTime.tm_min, dateTime.tm_sec);
}

uint32_t get_epoch(int year, int month, int day, int hour, int minute,
                   int second) {
    if (!is_valid_date(day, month, year) || !is_valid_time(hour, minute, second)) return 0;
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

long millis_since(unsigned long time_ms) {
    return millis_passed(time_ms, millis());
}

void epoch_to_tm(unsigned long epoch_s, struct tm &tm) {
    // since 1970
    uint32_t time;
    time = (uint32_t) epoch_s;

    tm.tm_sec = time % ONE_MINUTE_s;
    time = time / ONE_MINUTE_s;

    tm.tm_min = time % ONE_HOUR_m;
    time = time / ONE_HOUR_m;

    tm.tm_hour = time % ONE_DAY_h;
    time = time / ONE_DAY_h;

    tm.tm_wday = ((time + 4) % 7) + 1;

    uint8_t year = 0;
    unsigned long days = 0;
    while ((unsigned)(days += (is_leap_year(year) ? 366 : 365)) <= time) year++;
    tm.tm_year = year;

    time = time - (days - is_leap_year(year) ? 366 : 365);
    uint8_t monthN;
    uint8_t monthDays;
    for (monthN = 0; monthN < 12; monthN++) {
        if (monthN == 1) {
            if (is_leap_year(year)) {
                monthDays = 29;
            } else {
                monthDays = 28;
            }
        } else {
            monthDays = calendar[monthN].days;
        }
        if (time >= monthDays)
            time = time - monthDays;
        else
            break;
    }
    tm.tm_mon = monthN + 1;
    tm.tm_mday = time + 1;
}

unsigned long get_appbuild_epoch() {
    tm tm;
    char buf[16];
    strcpy(buf, BUILD_DATE);
    if (!str_to_date(buf, tm)) {
#ifdef DEBUG_TIME_UTILS
        USE_SERIAL.printf("[error] str_to_time:'%s'", buf);
        USE_SERIAL.println();
#endif
        return 0;
    }
    memset(&buf[0], 0, sizeof(buf[0]) * 16);
    strcpy(buf, BUILD_TIME);
    if (!str_to_time(buf, tm)) {
#ifdef DEBUG_TIME_UTILS
        USE_SERIAL.printf("[error] str_to_time: '%s'", buf);
        USE_SERIAL.println();
#endif
        return 0;
    }
    return get_epoch(tm);
}
#include "TimeUtils.h"

#include <Arduino.h>

#include "CommonTypes.h"

static const Month calendar[] = {
    {"Jan", 31, 0},   {"Feb", 28, 31},  {"Mar", 31, 59},  {"Apr", 30, 90},
    {"May", 31, 120}, {"Jun", 30, 151}, {"Jul", 31, 181}, {"Aug", 31, 212},
    {"Sep", 30, 273}, {"Oct", 31, 304}, {"Nov", 30, 334}, {"Dec", 31, 365}};

bool isLeapYear(uint16_t year) {
    return (((year) > 0) && !((year) % 4) &&
            (((year) % 100) || !((year) % 400)));
}

uint16_t encodeYear(uint16_t &year) {
    year += (year < 100) ? 1970 : 0;
    return (year >= 2019) ? year : 1970;
}

bool isValidMonth(uint8_t month) { return (month >= 1) && (month <= 12); }

bool isValidDay(uint8_t mday, uint8_t month, uint16_t year) {
    return (mday >= 1) && (mday <= getDaysInMonth(month, year));
}

bool isValidHour(uint8_t n) { return n >= 0 && n < 24; }

bool isValidMinute(uint8_t n) { return n >= 0 && n < 60; }

bool isValidSecond(uint8_t n) { return n >= 0 && n < 60; }

bool isValidDate(uint8_t day, uint8_t month, uint16_t year) {
    year = encodeYear(year);
    bool res = year && isValidMonth(month) && isValidDay(day, month, year);
    if (!res) {
        DEBUG.print(StrUtils::getStrP(str_invalid));
        DEBUG.print(StrUtils::getStrP(str_date));
        DEBUG.println();
    }
    return res;
}

bool isValidTime(uint8_t hour, uint8_t minute, uint8_t second) {
    bool res =
        isValidHour(hour) && isValidMinute(minute) && isValidSecond(second);
    if (!res) {
        DEBUG.print(StrUtils::getStrP(str_invalid));
        DEBUG.print(StrUtils::getStrP(str_time));
    }
    return res;
}

uint8_t getDaysInMonth(uint8_t month, uint16_t year) {
    return isValidMonth(month) ? (month == 1 ? (isLeapYear(year) ? 29 : 28)
                                             : calendar[month].mdays)
                               : 0;
}

uint16_t getDaysInTheYear(uint16_t year) {
    return isLeapYear(year) ? 366 : 365;
}

uint8_t encodeMonth(String str) {
    for (uint8_t n = 0; n < 11; n++)
        if (str.equalsIgnoreCase(calendar[n].name) == 1) return n + 1;
    return 0;
}

bool encodeTime(char *str, struct tm &tm) {
    tm.tm_hour = atoi(strtok(str, ":"));
    tm.tm_min = atoi(strtok(NULL, ":"));
    tm.tm_sec = atoi(str);
    if (!isValidTime(tm.tm_hour, tm.tm_min, tm.tm_sec)) {
        tm.tm_hour = 0;
        tm.tm_min = 0;
        tm.tm_sec = 0;
        return false;
    }
    return true;
}

bool encodeDate(char *str, struct tm &tm) {
    char buf[8];
    strcpy(buf, strtok(str, " "));        
    tm.tm_mon = encodeMonth(buf);
    tm.tm_mday = atoi(strtok(NULL, " "));
    tm.tm_year = atoi(strtok(NULL, " "));
    if (!isValidDate(tm.tm_mday, tm.tm_mon, tm.tm_year)) {
        tm.tm_mon = 1;
        tm.tm_mday = 1;
        tm.tm_year = 1970;
        return false;
    }
    return true;
}

uint32_t encodeEpoch(tm tm) {
    return DateTime(tm).asEpoch();
}

unsigned long millis_since(unsigned long sinse) {
    return millis_passed(sinse, millis());
}

unsigned long millis_passed(unsigned long start, unsigned long finish) {
    unsigned long result = 0;
    if (start <= finish) {
        unsigned long passed = finish - start;
        if (passed <= __LONG_MAX__) {
            result = static_cast<long>(passed);
        } else {
            result = static_cast<long>((__LONG_MAX__ - finish) + start + 1u);
        }
    } else {
        unsigned long passed = start - finish;
        if (passed <= __LONG_MAX__) {
            result = static_cast<long>(passed);
            result = -1 * result;
        } else {
            result = static_cast<long>((__LONG_MAX__ - start) + finish + 1u);
            result = -1 * result;
        }
    }
    return result;
}

void epochToDateTime(unsigned long epoch_s, struct tm &tm) {
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

    uint16_t year = 0;
    unsigned long days = 0;
    while ((days += getDaysInTheYear(year)) <= passed) year++;
    tm.tm_year = encodeYear(year);
    passed = passed - (days - getDaysInTheYear(tm.tm_year));
    uint8_t monthNumber;
    for (monthNumber = 1; monthNumber <= 12; monthNumber++) {
        uint8_t daysInMonth = getDaysInMonth(monthNumber, tm.tm_year);
        if (passed >= daysInMonth)
            passed -= daysInMonth;
        else
            break;
    }
    tm.tm_mon = monthNumber;
    tm.tm_mday = passed + 1;
#ifdef DEBUG_TIME_UTILS
    DEBUG.printf("year %d buf %d day %d weekday %d passed %02d:%02d:%02d \r\n",
                 tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_wday, tm.tm_hour,
                 tm.tm_min, tm.tm_sec);
#endif
}

unsigned long getAppBuildEpoch() {
    tm tm;
    char str[16];
    strcpy(str, BUILD_DATE);
    if (!encodeDate(str, tm)) {
#ifdef DEBUG_TIME_UTILS
        USE_SERIAL.printf("[error] str_to_date:'%s'", str);
        USE_SERIAL.println();
#endif
        return 0;
    }
    memset(&str[0], 0, sizeof(str[0]) * 16);
    strcpy(str, BUILD_TIME);
    if (!encodeTime(str, tm)) {
#ifdef DEBUG_TIME_UTILS
        USE_SERIAL.printf("[error] str_to_passed: '%s'", str);
        USE_SERIAL.println();
#endif
        return 0;
    }
    return encodeEpoch(tm) - (BUILD_TIMEZONE_h * ONE_HOUR_s);
}


size_t tmtodtf(struct tm &tm, char *str) {
    sprintf(str, DATETIME_FORMAT, tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour,
            tm.tm_min, tm.tm_sec);
    return strlen(str);
}


String getTimeStr(unsigned long now) {
    uint8_t hours = (now % 86400L) / 3600;
    uint8_t minutes = (now % 3600) / 60;
    uint8_t seconds = now % 60;
    char buf[16];
    sprintf_P(buf, strf_time, hours, minutes, seconds);
    return String(buf);
}

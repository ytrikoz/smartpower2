#include "time_utils.h"

uint8_t getMonthNum(String str) {
    for (uint8_t n = 0; n < 11; n++)
        if (str.equalsIgnoreCase(calendar[n].name) == 1) return n + 1;
    return 0;
}

bool isLeapYear(uint8_t year) {
    uint16_t today = 1970 + year;
    return (((today) > 0) && !((today) % 4) &&
            (((today) % 100) || !((today) % 400)));
}

bool isValidYear(int year) { return year > 1970 && year < 2040; }

bool isValidMonth(uint8_t n) { return (n >= 1) && (n <= 12); }

bool isValidHour(uint8_t hour) { return hour > 0 && hour < 24; }

bool isValidMinute(uint8_t minute) { return minute > 0 && minute < 60; }

bool isValidSecond(uint8_t second) { return second > 0 && second < 60; }

bool isValidDate(int year, int month, int day) {
    return (day > 0) && (day <= getDaysInMonth(month, year)) &&
           (isValidYear(year));
}

uint8_t getDaysInMonth(uint8_t month, uint16_t year) {
    if (!isValidMonth(month)) return 0;
    if (month == 1) return isLeapYear(year) ? 29 : 28;
    return calendar[month].days;
}

bool decodeTimeStr(char *str, struct tm &tm) {
    //uint8_t &hour, uint8_t &minute, uint8_t &second) 
    uint8_t items = sscanf("%2d:%2d:%2d", str, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);
    if (items == 2) {
        tm.tm_sec = 0;
        items++;
    }
    if (items == 3) {
        if (isValidHour(tm.tm_hour) && isValidMinute(tm.tm_min) && isValidSecond(tm.tm_sec))
            return true;
    }
    return false;
}

bool decodeDateStr(char *str, struct tm &tm) {
    char monthStr[8];
    uint8_t items = sscanf("%4s:%2d:%4d", str, monthStr, &tm.tm_mday, &tm.tm_year);
    if (items != 3) return false;
    tm.tm_mon = getMonthNum(monthStr);
    return isValidDate(tm.tm_year, tm.tm_mon, tm.tm_mday);
}

uint32_t getEpoch_s(int year, uint8_t month, uint8_t day, uint8_t hour,
                    uint8_t minute, uint8_t second) {
    if (year < 1970 || month > 12 || day > 31 || hour > 23 || minute > 59 ||
        second > 59)
        return 0;
    uint16_t month2date[] = {0,   31,  59,  90,  120, 151, 181,
                             212, 243, 273, 304, 334, 365};
    uint32_t unixdays = (year - 1970) * 365 + ((year - 1969) / 4);
    unixdays += month2date[month - 1] + (day - 1) +
                ((year % 4 == 0 && month > 2) ? 1 : 0);
    return unixdays * ONE_DAY_s + hour * ONE_HOUR_s + minute * ONE_MINUTE_s +
           second;
}

long timePassed(unsigned long start_ms, unsigned long finish_ms) {
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

long millisSince(unsigned long since_ms) {
    return timePassed(since_ms, millis());
}

void epochToDateTime(unsigned long epoch_s, struct tm &tm) {
    // since 1970
    uint32_t time;
    time = (uint32_t)epoch_s;

    tm.tm_sec = time % ONE_MINUTE_s;
    time = time / ONE_MINUTE_s;

    tm.tm_min = time % ONE_HOUR_m;
    time = time / ONE_HOUR_m;

    tm.tm_hour = time % ONE_DAY_h;
    time = time / ONE_DAY_h;

    tm.tm_wday = ((time + 4) % 7) + 1;

    uint8_t year = 0;
    unsigned long days = 0;
    while ((unsigned)(days += (isLeapYear(year) ? 366 : 365)) <= time) year++;
    tm.tm_year = year;

    time = time - (days - isLeapYear(year) ? 366 : 365);
    uint8_t monthN;
    uint8_t monthDays;
    for (monthN = 0; monthN < 12; monthN++) {
        if (monthN == 1) {
            if (isLeapYear(year)) {
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

unsigned long getAppBuildTime() {
    tm tm;
    char date[16];
    strcpy(date, BUILD_DATE);
    decodeDateStr(date, tm);
    char time[16];
    strcpy(time, BUILD_TIME);
    decodeTimeStr(time, tm);

    
    return 0;
}
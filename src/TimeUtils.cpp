#include "TimeUtils.h"

#include "Consts.h"
#include "StrUtils.h"

using namespace StrUtils;

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

namespace TimeUtils {

struct Month {
    const char *name;
    uint8_t mdays;
    uint16_t yday;
    Month(const char *name, uint8_t mdays, uint16_t yday) {
        this->name = name;
        this->mdays = mdays;
        this->yday = yday;
    }
    unsigned long toEpoch() { return this->mdays * ONE_DAY_s; }
};

static const Month calendar[] = {
    {"Jan", 31, 0}, {"Feb", 28, 31}, {"Mar", 31, 59}, {"Apr", 30, 90}, {"May", 31, 120}, {"Jun", 30, 151}, {"Jul", 31, 181}, {"Aug", 31, 212}, {"Sep", 30, 273}, {"Oct", 31, 304}, {"Nov", 30, 334}, {"Dec", 31, 365}};

// 2019
bool isLeapYear(uint16_t year) {
    return (((year) > 0) && !((year) % 4) &&
            (((year) % 100) || !((year) % 400)));
}

bool isValidMonth(uint8_t month) { return (month >= 1) && (month <= 12); }

bool isValidDay(uint8_t mday, uint8_t month, uint16_t year) {
    return (mday >= 1) && (mday <= daysInMonth(month, year));
}

bool isValidHour(uint8_t n) { return n >= 0 && n < 24; }

bool isValidMinute(uint8_t n) { return n >= 0 && n < 60; }

bool isValidSecond(uint8_t n) { return n >= 0 && n < 60; }

bool isValidDate(uint8_t day, uint8_t month, uint16_t year) {
    return year && isValidMonth(month) && isValidDay(day, month, year);
}

bool isValidTime(uint8_t hour, uint8_t minute, uint8_t second) {
    return isValidHour(hour) && isValidMinute(minute) && isValidSecond(second);
}

uint8_t daysInMonth(uint8_t month, uint16_t year) {
    return isValidMonth(month) ? (month == 1 ? (isLeapYear(year) ? 29 : 28)
                                             : calendar[month].mdays)
                               : 0;
}

uint16_t daysInYear(uint16_t year) { return isLeapYear(year) ? 366 : 365; }

uint16_t encodeYear(uint16_t &year) {
    if (year <= 99)
        year += 1970;
    return year;
}

bool encodeMonth(String str, int &month) {
    for (month = 0; month < 11; ++month)
        if (str.equalsIgnoreCase(calendar[month].name) == 1)
            return true;
    return false;
}

bool encodeTime(const char *str, struct tm &tm) {
#ifdef DEBUG_TIME_UTILS
    DEBUG.printf("[encodeTime] %s", str);
    DEBUG.println();
#endif
    char buf[16];
    strncpy(buf, str, 16);
    tm.tm_hour = atoi(strtok(buf, ":"));
    tm.tm_min = atoi(strtok(NULL, ":"));
    tm.tm_sec = atoi(buf);
#ifdef DEBUG_TIME_UTILS
    DEBUG.printf("[encodeTime] %d %d %d", tm.tm_hour, tm.tm_min, tm.tm_sec);
    DEBUG.println();
#endif
    return true;
    // isValidTime(tm.tm_hour, tm.tm_min, tm.tm_sec);
}

bool encodeDate(char *str, struct tm &tm) {
#ifdef DEBUG_TIME_UTILS
    DEBUG.printf("[encodeDate] %s", str);
    DEBUG.println();
#endif
    char buf[8];
    strcpy(buf, strtok(str, " "));
    encodeMonth(buf, tm.tm_mon);
    tm.tm_mday = atoi(strtok(NULL, " "));
    tm.tm_year = atoi(strtok(NULL, " ")) - 1900;
#ifdef DEBUG_TIME_UTILS
    DEBUG.printf("[encodeDate] %d %d %d", tm.tm_mon, tm.tm_mday, tm.tm_year);
    DEBUG.println();
#endif
    return true;
    // isValidDate(tm.tm_mday, tm.tm_mon, tm.tm_year);
}

size_t tmtodtf(struct tm &tm, char *str) {
    sprintf(str, DATETIME_FORMAT, tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour,
            tm.tm_min, tm.tm_sec);
    return strlen(str);
}

void format_elapsed_time(char *buf, time_t elapsed) {
    int h, m;
    h = m = 0;
    h = elapsed / 3600;
    elapsed -= (h * 3600);
    m = elapsed / 60;
    elapsed -= (m * 60);
    sprintf(buf, "%02i:%02i:%02lu", h, m, elapsed);
}

void format_elapsed_time(char *buf, double elapsed) {
    int h, m, s, ms;
    h = m = s = ms = 0;
    ms = elapsed * 1000;
    h = ms / 3600000;
    ms -= (h * 3600000);
    m = ms / 60000;
    ms -= (m * 60000);
    s = ms / 1000;
    ms -= (s * 1000);
    sprintf(buf, "%02i:%02i:%02i.%03i", h, m, s, ms);
}


int timeZoneInSeconds(const byte timeZone) {
    int res = 0;
    switch (constrain(timeZone, 1, 38)) {
        case 1:
            res = -12 * ONE_HOUR_s;
            break;
        case 2:
            res = -11 * ONE_HOUR_s;
            break;
        case 3:
            res = -10 * ONE_HOUR_s;
            break;
        case 4:
            res = -9 * ONE_HOUR_s - 30 * ONE_MINUTE_s;
            break;
        case 5:
            res = -9 * ONE_HOUR_s;
            break;
        case 6:
            res = -8 * ONE_HOUR_s;
            break;
        case 7:
            res = -7 * ONE_HOUR_s;
            break;
        case 8:
            res = -6 * ONE_HOUR_s;
            break;
        case 9:
            res = -5 * ONE_HOUR_s;
            break;
        case 10:
            res = -4 * ONE_HOUR_s;
            break;
        case 11:
            res = -3 * ONE_HOUR_s - 30 * ONE_MINUTE_s;
            break;
        case 12:
            res = -3 * ONE_HOUR_s;
            break;
        case 13:
            res = -2 * ONE_HOUR_s;
            break;
        case 14:
            res = -1 * ONE_HOUR_s;
            break;
        case 15:
            res = 0;
            break;
        case 16:
            res = 1 * ONE_HOUR_s;
            break;
        case 17:
            res = 2 * ONE_HOUR_s;
            break;
        case 18:
            res = 3 * ONE_HOUR_s;
            break;
        case 19:
            res = 3 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
            break;
        case 20:
            res = 4 * ONE_HOUR_s;
            break;
        case 21:
            res = 4 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
            break;
        case 22:
            res = 5 * ONE_HOUR_s;
            break;
        case 23:
            res = 5 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
            break;
        case 24:
            res = 5 * ONE_HOUR_s + 45 * ONE_MINUTE_s;
            break;
        case 25:
            res = 6 * ONE_HOUR_s;
            break;
        case 26:
            res = 6 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
            break;
        case 27:
            res = 7 * ONE_HOUR_s;
            break;
        case 28:
            res = 8 * ONE_HOUR_s;
            break;
        case 29:
            res = 8 * ONE_HOUR_s + 45 * ONE_MINUTE_s;
            break;
        case 30:
            res = 9 * ONE_HOUR_s;
            break;
        case 31:
            res = 9 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
            break;
        case 32:
            res = 10 * ONE_HOUR_s;
            break;
        case 33:
            res = 10 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
            break;
        case 34:
            res = 11 * ONE_HOUR_s;
            break;
        case 35:
            res = 12 * ONE_HOUR_s;
            break;
        case 36:
            res = 12 * ONE_HOUR_s + 45 * ONE_MINUTE_s;
            break;
        case 37:
            res = 13 * ONE_HOUR_s;
            break;
        case 38:
            res = 14 * ONE_HOUR_s;
            break;
    }
    return res;
}

}  // namespace TimeUtils

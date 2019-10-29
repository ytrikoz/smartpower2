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
    {"Jan", 31, 0},   {"Feb", 28, 31},  {"Mar", 31, 59},  {"Apr", 30, 90},
    {"May", 31, 120}, {"Jun", 30, 151}, {"Jul", 31, 181}, {"Aug", 31, 212},
    {"Sep", 30, 273}, {"Oct", 31, 304}, {"Nov", 30, 334}, {"Dec", 31, 365}};

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
    USE_SERIAL.printf("[encodeTime] %s", str);
    USE_SERIAL.println();
#endif
    char buf[16];
    strncpy(buf, str, 16);
    tm.tm_hour = atoi(strtok(buf, ":"));
    tm.tm_min = atoi(strtok(NULL, ":"));
    tm.tm_sec = atoi(buf);
#ifdef DEBUG_TIME_UTILS
    USE_SERIAL.printf("[encodeTime] %d %d %d", tm.tm_hour, tm.tm_min,
                      tm.tm_sec);
    USE_SERIAL.println();
#endif
    return true;
    // isValidTime(tm.tm_hour, tm.tm_min, tm.tm_sec);
}

bool encodeDate(char *str, struct tm &tm) {
#ifdef DEBUG_TIME_UTILS
    USE_SERIAL.printf("[encodeDate] %s", str);
    USE_SERIAL.println();
#endif
    char buf[8];
    strcpy(buf, strtok(str, " "));
    encodeMonth(buf, tm.tm_mon);
    tm.tm_mday = atoi(strtok(NULL, " "));
    tm.tm_year = atoi(strtok(NULL, " ")) - 1900;
#ifdef DEBUG_TIME_UTILS
    USE_SERIAL.printf("[encodeDate] %d %d %d", tm.tm_mon, tm.tm_mday,
                      tm.tm_year);
    USE_SERIAL.println();
#endif
    return true;
    // isValidDate(tm.tm_mday, tm.tm_mon, tm.tm_year);
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
    while ((days += daysInYear(year)) <= passed)
        year++;
    tm.tm_year = encodeYear(year);
    passed = passed - (days - daysInYear(tm.tm_year));
    uint8_t month;
    for (month = 1; month <= 12; month++) {
        uint8_t daym = daysInMonth(month, tm.tm_year);
        if (passed >= daym)
            passed -= daym;
        else
            break;
    }
    tm.tm_mon = month;
    tm.tm_mday = passed + 1;
#ifdef DEBUG_TIME_UTILS
    DEBUG.printf("year %d buf %d day %d weekday %d passed %02d:%02d:%02d \r\n",
                 tm.tm_year, tm.tm_mon, tm.tm_mday, tm.tm_wday, tm.tm_hour,
                 tm.tm_min, tm.tm_sec);
#endif
}

size_t tmtodtf(struct tm &tm, char *str) {
    sprintf(str, DATETIME_FORMAT, tm.tm_mday, tm.tm_mon, tm.tm_year, tm.tm_hour,
            tm.tm_min, tm.tm_sec);
    return strlen(str);
}

unsigned long getAppBuildUtc() {
    tm tm;
    char buf[16];
    strcpy(buf, BUILD_DATE);
    encodeDate(buf, tm);
    strcpy(buf, BUILD_TIME);
    encodeTime(buf, tm);
    return DateTime(tm).asEpoch() - BUILD_TIMEOFFSET_h * ONE_HOUR_s;
}

sint32_t timeZoneToOffset(const uint8_t timeZone) {
    sint32_t result = 0;
    switch (constrain(timeZone, 1, 38)) {
    case 1:
        result = -12 * ONE_HOUR_s;
        break;
    case 2:
        result = -11 * ONE_HOUR_s;
        break;
    case 3:
        result = -10 * ONE_HOUR_s;
        break;
    case 4:
        result = -9 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
        break;
    case 5:
        result = -9 * ONE_HOUR_s;
        break;
    case 6:
        result = -8 * ONE_HOUR_s;
        break;
    case 7:
        result = -7 * ONE_HOUR_s;
        break;
    case 8:
        result = -6 * ONE_HOUR_s;
        break;
    case 9:
        result = -5 * ONE_HOUR_s;
        break;
    case 10:
        result = -4 * ONE_HOUR_s;
        break;
    case 11:
        result = -3 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
        break;
    case 12:
        result = -3 * ONE_HOUR_s;
        break;
    case 13:
        result = -2 * ONE_HOUR_s;
        break;
    case 14:
        result = -1 * ONE_HOUR_s;
        break;
    case 15:
        result = 0;
        break;
    case 16:
        result = 1 * ONE_HOUR_s;
        break;
    case 17:
        result = 2 * ONE_HOUR_s;
        break;
    case 18:
        result = 3 * ONE_HOUR_s;
        break;
    case 19:
        result = 3 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
        break;
    case 20:
        result = 4 * ONE_HOUR_s;
        break;
    case 21:
        result = 4 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
        break;
    case 22:
        result = 5 * ONE_HOUR_s;
        break;
    case 23:
        result = 5 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
        break;
    case 24:
        result = 5 * ONE_HOUR_s + 45 * ONE_MINUTE_s;
        break;
    case 25:
        result = 6 * ONE_HOUR_s;
        break;
    case 26:
        result = 6 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
        break;
    case 27:
        result = 7 * ONE_HOUR_s;
        break;
    case 28:
        result = 8 * ONE_HOUR_s;
        break;
    case 29:
        result = 8 * ONE_HOUR_s + 45 * ONE_MINUTE_s;
        break;
    case 30:
        result = 9 * ONE_HOUR_s;
        break;
    case 31:
        result = 9 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
        break;
    case 32:
        result = 10 * ONE_HOUR_s;
        break;
    case 33:
        result = 10 * ONE_HOUR_s + 30 * ONE_MINUTE_s;
        break;
    case 34:
        result = 11 * ONE_HOUR_s;
        break;
    case 35:
        result = 12 * ONE_HOUR_s;
        break;
    case 36:
        result = 12 * ONE_HOUR_s + 45 * ONE_MINUTE_s;
        break;
    case 37:
        result = 13 * ONE_HOUR_s;
        break;
    case 38:
        result = 14 * ONE_HOUR_s;
        break;
    }
    return result;
}

} // namespace TimeUtils

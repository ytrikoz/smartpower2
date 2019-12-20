#include "Utils/TimeUtils.h"

#include <time.h>

#include "Consts.h"

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

char elapsed_buf[32] = {0};
char* format_elapsed_short(const unsigned long elapsed) {
    int h, m, s = 0;
    s = elapsed / 1000;  
    h = s / 3600;
    s -= (h * 3600);
    m = s / 60;
    s -= (m * 60);
    sprintf(elapsed_buf, "%02i:%02i:%02i", h, m, s);
    return elapsed_buf;
}

char* format_elapsed_full(const double elapsed) {
    int h, m, s, ms;
    h = m = s = ms = 0;
    ms = elapsed * 1000;
    h = ms / 3600000;
    ms -= (h * 3600000);
    m = ms / 60000;
    ms -= (m * 60000);
    s = ms / 1000;
    ms -= (s * 1000);
    sprintf(elapsed_buf, "%02i:%02i:%02i.%03i", h, m, s, ms);
    return elapsed_buf;
}

char* format_time(const time_t time) {
    char* buf = ctime(&time);
    size_t len = strlen(buf);
    buf[len - 1] = '\x00';    
    return buf;
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

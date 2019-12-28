#include "Utils/StrUtils.h"

namespace StrUtils {

static const char strf_network[] PROGMEM = "ip: %s subnet: %s gateway: %s";
static const char strf_network_dns[] PROGMEM =
    "ip: %s subnet: %s gateway: %s dns: %s";

static const char *weekdays[] = {"sun", "mon", "tue", "wed",
                                 "thu", "fri", "sat"};

static BoolSet bools[4] = {
    {str_true, str_false},
    {str_enabled, str_disabled},
    {str_up, str_down},
    {str_on, str_off}
};

String getBoolStr(bool value, BoolStrEnum set) {
    return (FPSTR(value ? bools[set].true_ : bools[set].false_));    
}

String getTimeStr(const unsigned long epoch_s, bool fmtLong) {
    // seconds since 1970-01-01 00:00:00
    unsigned long passed = epoch_s;
    uint8_t seconds = passed % ONE_MINUTE_s;
    passed /= ONE_MINUTE_s;
    uint8_t minutes = passed % ONE_HOUR_m;
    passed /= ONE_HOUR_m;
    uint8_t hours = passed % ONE_DAY_h;
    passed /= ONE_DAY_h;
    char buf[32];
    if (fmtLong) {
        int8_t weekday = ((passed + 4) % 7);
        sprintf_P(buf, TIME_LONG_FORMAT, weekdays[weekday], hours, minutes,
                  seconds);
    } else {
        sprintf_P(buf, TIME_FORMAT, hours, minutes, seconds);
    }
    return String(buf);
}

void strfill(char *str, char chr, size_t len) {
    memset(&str[0], chr, sizeof(char) * len);
    str[len - 1] = '\x00';
}

IPAddress atoip(const char *str) {
    if (!isip(str))
        return IPAddress(IPADDR_NONE);
    uint8_t parts[4] = {0, 0, 0, 0};
    uint8_t part = 0;
    for (uint8_t a = 0; a < strlen(str); a++) {
        uint8_t b = str[a];
        if (b == '.') {
            part++;
            continue;
        }
        parts[part] *= 10;
        parts[part] += b - '0';
    }
    return IPAddress(parts[0], parts[1], parts[2], parts[3]);
}  // namespace StrUtils

void setnnstr(char *dest, const char *src) {
    if (src == NULL) {
        dest = new char[1];
        dest[0] = '\x00';
    } else {
        size_t len = strlen(src);
        dest = new char[len + 1];
        setstr(dest, src, len);
    }
}

bool setstr(char *dest, const char *src, size_t size) {
    if (src == NULL) {
        memset(dest, 0, size);
    } else if (strcmp(src, dest) != 0) {
        uint8_t len = strlen(src);
        if (len != 0) {
            if (len > size - 1)
                len = size - 1;
            memcpy(dest, src, len);
            dest[len] = '\x00';
        }
        return true;
    }
    return false;
}

String fmt_network(const IPAddress ipaddr, const IPAddress subnet,
                   const IPAddress gateway) {
    char buf[128];
    sprintf_P(buf, strf_network, ipaddr.toString().c_str(),
              subnet.toString().c_str(), gateway.toString().c_str());
    return String(buf);
}

String fmt_network(const IPAddress ipaddr, const IPAddress subnet,
                   const IPAddress gateway, const IPAddress dns) {
    char buf[128];
    sprintf_P(buf, strf_network_dns, ipaddr.toString().c_str(),
              subnet.toString().c_str(), gateway.toString().c_str(),
              dns.toString().c_str());
    return String(buf);
}

void strpadd(char *str, Align align, size_t size, const char ch) {
    uint8_t str_len = strlen(str);
    if (str_len > size)
        str_len = size;
    char orig_str[str_len + 1];
    strncpy(orig_str, str, str_len);
    strfill(str, ch, size + 1);
    uint8_t str_start = 0;
    switch (align) {
        // [...str]
        case RIGHT:
            str_start = size - str_len;
            break;
        // [..str..]
        case CENTER:
            str_start = floor((float)(size - str_len) / 2);
            break;
        // [str...]
        case LEFT:
        default:
            str_start = 0;
    }
    for (size_t i = 0; i < str_len; ++i)
        str[i + str_start] = orig_str[i];
}

// http://stackoverflow.com/a/35236734
void stringToBytes(const char *str, char sep, uint8_t *size, int len,
                   int base) {
    for (int i = 0; i < len; i++) {
        size[i] = strtoul(str, NULL, base);
        str = strchr(str, sep);
        if (str == NULL || *str == '\x00') {
            break;
        }
        str++;
    }
}

bool isip(const char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
        int c = str[i];
        if (c != '.' && (c < '0' || c > '9'))
            return false;
    }
    return true;
}

bool isip(const String &str) { return isip(str.c_str()); }

String getOnOffStr(bool value, bool space) {
    String res(FPSTR(value ? str_on : str_off));
    return res;
}

String getEnabledStr(bool value, bool space) {
    String res(FPSTR(value ? str_enabled : str_disabled));
    return res;
}

String getUpDownStr(bool value) {
    return String(FPSTR(value ? str_up : str_down));
}

}  // namespace StrUtils

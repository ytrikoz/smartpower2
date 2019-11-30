#include "StrUtils.h"

#include "TimeUtils.h"

namespace StrUtils {

static const char strf_network[] PROGMEM = "ip: %s subnet: %s gateway: %s";
static const char strf_network_dns[] PROGMEM =
    "ip: %s subnet: %s gateway: %s dns: %s";
static const char str_down[] PROGMEM = "down";
static const char str_up[] PROGMEM = "up";
static const char *weekdays[] = {"sun", "mon", "tue", "wed",
                                 "thu", "fri", "sat"};

bool atomac(const char *txt, uint8_t *addr) {
    for (uint8_t i = 0; i < 6; i++) {
        int a, b;
        a = hex2num(*txt++);
        if (a < 0)
            return false;
        b = hex2num(*txt++);
        if (b < 0)
            return false;
        *addr++ = (a << 4) | b;
        if (i < 5 && *txt++ != ':')
            return false;
    }
    return true;
}

int hex2num(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

bool atomac(const char *txt, uint8_t *addr);

int hex2byte(const char *hex) {
    int a, b;
    a = hex2num(*hex++);
    if (a < 0)
        return -1;
    b = hex2num(*hex++);
    if (b < 0)
        return -1;
    return (a << 4) | b;
}

String asJsonObj(const char *key, const char *value) {
    char buf[128];
    sprintf(buf, "{\"%s\":\"%s\"}", key, value);
    return String(buf);
}

String asJsonObj(const char *key, String value) {
    return asJsonObj(key, value.c_str());
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

String getDateTimeFormated(const unsigned long epoch_s) {
    // seconds since 1970-01-01 00:00:00
    unsigned long passed = epoch_s;
    uint8_t second = passed % ONE_MINUTE_s;
    passed = passed / ONE_MINUTE_s;

    uint8_t minute = passed % ONE_HOUR_m;
    passed = passed / ONE_HOUR_m;

    uint8_t hour = passed % ONE_DAY_h;
    passed = passed / ONE_DAY_h;

    uint16_t year = 0;
    unsigned long days = 0;
    while ((days += TimeUtils::daysInYear(year)) <= passed)
        year++;
    year = TimeUtils::encodeYear(year);

    passed -= days - TimeUtils::daysInYear(year);
    uint8_t month;
    for (month = 1; month <= 12; ++month) {
        uint8_t daysInMonth = TimeUtils::daysInMonth(month, year);
        if (passed >= daysInMonth)
            passed -= daysInMonth;
        else
            break;
    }
    uint8_t day = passed + 1;
    char buf[32];
    sprintf_P(buf, DATETIME_FORMAT, day, month, year, hour, minute, second);
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
} // namespace StrUtils

String iptos(IPAddress &ip) { return ip.toString(); }

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

const String fmt_ip_port(const IPAddress &ip, const uint16_t port) {
    return fmt_ip_port(ip.toString().c_str(), port);
}

const String fmt_ip_port(const char *ip, const uint16_t port) {
    char buf[32];
    sprintf(buf, "%s:%d", ip, port);
    return String(buf);
}

String fmt_size(size_t size) {
    if (size < 1024)
        return String(size) + "b";
    else if (size < (1024 * 1024))
        return String(size / 1024.0) + "kB";
    else if (size < (1024 * 1024 * 1024))
        return String(size / 1024.0 / 1024.0) + "MB";
    else
        return String(size / 1024.0 / 1024.0 / 1024.0) + "GB";
}

String fmt_rssi(int db) {
    char buf[32];
    sprintf(buf, "%d dB", db);
    return String(buf);
}

String fmt_mac(const uint8_t *mac) {
    char buf[32];
    sprintf(buf, MACSTR, MAC2STR(mac));
    return String(buf);
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
    sprintf(buf, strf_network_dns, ipaddr.toString().c_str(),
            subnet.toString().c_str(), gateway.toString().c_str(),
            dns.toString().c_str());
    return String(buf);
}

String fmt_mhz(uint32_t value) {
    char buf[8];
    itoa(value / ONE_MHz_hz, buf, DEC);
    strcat(buf, "MHz");
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

String getStr(String &str) { return str + " "; }

String getStr(IPAddress &value) {
    String res = value.toString() + " ";
    return res;
}

String getStr(long unsigned int value) {
    String res(value);
    return res;
}

String getStr(const char *str) {
    String res(str);
    return res + " ";
}

String getStr(int num) {
    String res(num, DEC);
    return res + " ";
}

String getBoolStr(bool value, bool space) {
    return String(value ? F("true") : F("false"));
}

String getOnOffStr(bool value, bool space) {
    String res(value ? F("on") : F("off"));
    return res;
}

String getEnabledStr(bool value, bool space) {
    String res(value ? F("enabled") : F("disabled"));
    return res;
}

char *getUpDownStr(char *buf, bool value, bool space) {
    PGM_P strP = value ? str_up : str_down;
    strcpy_P(buf, strP);
    return buf;
}

String getUpDownStr(bool value) {
    PGM_P strP = value ? str_up : str_down;
    return String(FPSTR(strP));
}

String getIdentStrP(PGM_P strP, bool with_space) {
    String str = getStrP(strP, false);
    return getIdentStr(str, with_space);
}

String getQuotedStrP(PGM_P strP, bool with_space, char ch) {
    char buf[64];
    strcpy_P(buf, strP);
    return getQuotedStr(buf, with_space);
}

String getQuotedStr(const char *str, bool with_space, char ch) {
    return getIdentStr(str, with_space, ch, ch);
}

String getQuotedStr(String &str, bool with_space) {
    return getQuotedStr(str.c_str(), with_space, '\'');
}

size_t getStrP(PGM_P strP, char *buf) {
    strcpy_P(buf, strP);
    return strlen(buf);
}

String getStrP(PGM_P strP, bool spaced) {
    char buf[64];
    strcpy_P(buf, strP);
    String str(buf);
    return str;
}

String getIdentStr(const char *str, bool with_space, char ch) {
    return getIdentStr(str, with_space, ch, ch);
}

String getIdentStr(String &str, bool with_space) {
    return getIdentStr(str.c_str(), with_space);
}

String getIdentStr(const char *str, bool with_space) {
    return getIdentStr(str, with_space, '[', ']');
}

String getIdentStr(const char *str, bool with_space, char left, char right) {
    char buf[32] = {0};
    buf[0] = left;
    strcpy(&buf[1], str);
    size_t x = strlen(buf);
    buf[x] = right;
    if (with_space)
        buf[++x] = ' ';
    return String(buf);
}

template <typename... Args> void sayArgsP(Print *p, Args... args) {
    char buf[128];
    for (size_t n = 0; n < sizeof...(args); ++n) {
        strcpy(buf, (char *)pgm_read_ptr(&args[n]...));
        p->print(buf);
    }
}

} // namespace StrUtils

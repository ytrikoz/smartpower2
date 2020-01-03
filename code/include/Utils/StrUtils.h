#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include "Strings.h"
#include "Consts.h"

namespace StrUtils {

enum BoolEnum {
    TRUE_FALSE, ENABLED_DISABLED, UP_DONW, ON_OFF
};

enum Align { LEFT,
             CENTER,
             RIGHT };

inline String bool2str(bool value, BoolEnum set = TRUE_FALSE) {
    struct BoolSet {
        PGM_P true_;
        PGM_P false_;
    };
    BoolSet bools[4] = {
        {str_true, str_false},
        {str_enabled, str_disabled},
        {str_up, str_down},
        {str_on, str_off}};
    return (FPSTR(value ? bools[set].true_ : bools[set].false_));
}

inline bool setstr(char *dest, const char *src, size_t size) {
    bool res = false;
    if (src == NULL || strlen(src) == 0) {
        memset(dest, 0, size);
    } else if (strcmp(src, dest) != 0) {
        size_t len = strlen(src);
        if (len != 0) {
            if (len > size - 1)
                len = size - 1;
            memcpy(dest, src, len);
            dest[len] = '\x00';
        }
        res = true;
    }
    return res;
}

inline void strfill(char *str, char chr, size_t len) {
    memset(&str[0], chr, sizeof(char) * len);
    str[len - 1] = '\x00';
}

inline void strpadd(char *str, Align align, size_t size, const char ch = ' ') {
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
inline void str2bytes(const char *str, char sep, uint8_t *size, int len,
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

inline int hex2num(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

inline int hex2byte(const char *hex) {
    int a, b;
    a = hex2num(*hex++);
    if (a < 0)
        return -1;
    b = hex2num(*hex++);
    if (b < 0)
        return -1;
    return (a << 4) | b;
}

inline String prettyMhz(uint32_t value) {
    char buf[8];
    itoa(value / ONE_MHz_hz, buf, DEC);
    strcat(buf, "MHz");
    return String(buf);
}

inline String prettyIp(IPAddress ip, uint16_t port) {
    char buf[32];
    sprintf(buf, "%s:%d", ip.toString().c_str(), port);
    return String(buf);
}

inline String prettyRssi(int db) {
    String res = String(db);
    res += " dB";
    return res;
}

inline String prettyBytes(size_t size) {
    if (size < 1024)
        return String(size) + "b";
    else if (size < (1024 * 1024))
        return String(size / 1024.0) + "kB";
    else if (size < (1024 * 1024 * 1024))
        return String(size / 1024.0 / 1024.0) + "MB";
    else
        return String(size / 1024.0 / 1024.0 / 1024.0) + "GB";
}

inline String prettyMac(const uint8_t mac[6]) {
    char buf[32];
    sprintf(buf, MACSTR, MAC2STR(mac));
    return String(buf);
}

inline String prettyNetwork(const IPAddress ipaddr, const IPAddress subnet, const IPAddress gateway) {
    String res;
    res = "ip: ";
    res += ipaddr.toString();
    res += "mask: ";
    res += subnet.toString();
    res += "gateway: ";
    res += gateway.toString();
    return res;
}

inline String prettyNetwork(const IPAddress ipaddr, const IPAddress subnet, const IPAddress gateway, const IPAddress dns) {
    String res = prettyNetwork(ipaddr, subnet, gateway);
    res += "dns: ";
    res += dns.toString();
    return res;
}

inline bool isip(const char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
        int c = str[i];
        if (c != '.' && (c < '0' || c > '9'))
            return false;
    }
    return true;
}

inline IPAddress str2ip(const char *str) {
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
}

inline bool str2mac(const char *buf, uint8_t *addr) {
    for (uint8_t i = 0; i < 6; i++) {
        int a, b;
        a = hex2num(*buf++);
        if (a < 0)
            return false;
        b = hex2num(*buf++);
        if (b < 0)
            return false;
        *addr++ = (a << 4) | b;
        if (i < 5 && *buf++ != ':')
            return false;
    }
    return true;
}

inline String double2str(double value) {
    char buf[64];
    sprintf(buf, "%.6f", value);
    return String(buf);
}

inline String long2str(long value) {
    char buf[64];
    sprintf(buf, "%ld", value);
    return String(buf);
}

template <typename T>
T concat(T v) { return v; }

template <typename T, typename... Args>
String concat(T first, Args... args) {
    return first + " " + concat(args...);
}

}  // namespace StrUtils

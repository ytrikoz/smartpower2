#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include <CommonTypes.h>

namespace StrUtils {

enum Align { LEFT,
             CENTER,
             RIGHT };

void stringToBytes(const char *str, char sep, uint8_t *bytes, int len,
                   int base);

IPAddress atoip(const char *);

bool isip(const char *);

bool isip(const String &);

bool setstr(char *dest, const char *src, size_t size);

void strfill(char *str, char chr, size_t size);

String fmt_network(const IPAddress ipddr, const IPAddress subnet,
                   const IPAddress gateway);

String fmt_network(const IPAddress ipaddr, const IPAddress subnet,
                   const IPAddress gateway, const IPAddress dns);

inline String prettyMhz(uint32_t value) {
    char buf[8];
    itoa(value / ONE_MHz_hz, buf, DEC);
    strcat(buf, "MHz");
    return String(buf);
}

inline String prettyRssi(int db) {
    String res = String(db);
    res += " dB";
    return res;
}

inline String prettyMac(const uint8_t mac[6]) {
    char buf[32];
    sprintf(buf, MACSTR, MAC2STR(mac));
    return String(buf);
}

void strpadd(char *str, Align align, size_t size, const char ch = ' ');

String getBoolStr(bool value, BoolStrEnum set = TRUE_FALSE);

inline String getQuotedStr(const char* str) {
    String res;
    res += "\'";
    res += str;
    res += "\'";
    return res;
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

inline bool atomac(const char *txt, uint8_t *addr) {
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


inline String prettyIp(IPAddress ip, uint16_t port) {
    char buf[32];
    sprintf(buf, "%s:%d", ip.toString().c_str(), port);
    return String(buf);
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

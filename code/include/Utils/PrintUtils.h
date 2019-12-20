#pragma once

#include "Arduino.h"

#include "Strings.h"
#include "StrUtils.h"

namespace PrintUtils {

inline size_t print(Print *p, const __FlashStringHelper *ifsh) {
    size_t n = 0;
    if (p) {
        n += p->print(ifsh);
        n += p->print(' ');
    }
    return n;
}

template <typename T>
inline size_t print_quoted(Print *p, T value) {
    size_t n = 0;
    if (p) {
        n += p->print('\'');
        n += p->print(value);
        n += p->print('\'');
        n += p->print(' ');
    }
    return n;
}

template <typename T>
inline size_t print_ident(Print *p, T value) {
    size_t n = 0;
    if (p) {
        n += p->print('[');
        n += p->print(value);
        n += p->print(']');
        n += p->print(' ');
    }
    return n;
}

template <typename T>
size_t print(Print *p, T v) {
    size_t n = 0;
    if (p) {
        n += p->print(v);
        n += p->print(' ');
    }
    return n;
}

template <typename T, typename... Args>
size_t print(Print *p, T first, Args... args) {
    size_t n = 0;
    if (p) {
        n += p->print(first);
        n += p->print(' ');
        n += print(p, args...);
    }
    return n;
}

inline size_t println(Print *p) {
    size_t n = 0;
    if (p) {
        n += p->println();
    }
    return n;
}

inline size_t println(Print *p, const __FlashStringHelper *ifsh) {
    size_t n = 0;
    n += print(p, ifsh);
    n += println(p);
    return n;
}

inline size_t print_not_found(Print *p, const String &str) {
    size_t n = 0;
    n += print_quoted(p, str);
    n += print(p, FPSTR(str_not), FPSTR(str_found));
    n += println(p);
    return n;
}

inline size_t print_file_not_found(Print *p, const String &name) {
    size_t n = 0;
    n += print(p, FPSTR(str_file));
    n += print_not_found(p, name);
    return n;
}

inline size_t print_dir_not_found(Print *p, const String &name) {
    size_t n = 0;
    n += print(p, FPSTR(str_dir));
    n += print_not_found(p, name);
    return n;
}

inline size_t print_unknown(Print *p, const String &str) {
    size_t n = 0;
    n += print(p, FPSTR(str_unknown));
    n += print_quoted(p, str);
    return n;
}

inline size_t println_unknown_param(Print *p, const String &name) {
    size_t n = 0;
    n += print_unknown(p, name);
    n += print(p, FPSTR(str_param));
    n += println(p);
    return n;
}

inline size_t println_unknown_module(Print *p, const String &name) {
    size_t n = 0;
    n += print_unknown(p, name);
    n += print(p, FPSTR(str_module));
    n += println(p);
    return n;
}

inline size_t println_unknown_action(Print *p, const String &name) {
    size_t n = 0;
    n += print_unknown(p, name);
    n += print(p, FPSTR(str_action));
    n += println(p);
    return n;
}

inline size_t println_done(Print *p) {
    size_t n = 0;
    n += print(p, FPSTR(str_done));
    n += println(p);
    return n;
}

inline size_t print_wifi_ap_probe(Print *p, const uint8_t mac[6], const int rssi) {
    size_t n = 0;
    n += print_ident(p, FPSTR(str_wifi_ap));
    n += print(p, FPSTR(str_probe));
    n += print(p, FPSTR(str_mac), StrUtils::prettyMac(mac));
    n += print(p, FPSTR(str_rssi), StrUtils::prettyRssi(rssi));
    n += println(p);
    return n;
}

inline size_t print_wifi_ap_station(Print *p, const uint8_t aid, const uint8_t mac[6], const bool connected) {
    size_t n = 0;
    n += print_ident(p, FPSTR(str_wifi_ap));
    n += print(p, connected ? FPSTR(str_connected) : FPSTR(str_disconnected));
    n += print(p, FPSTR(str_station), String(aid));
    n += print(p, FPSTR(str_mac), StrUtils::prettyMac(mac));
    n += println(p);
    return n;
}

inline size_t print_wifi_ap_station(Print *p, const uint8_t aid, const uint8_t mac[6]) {
    size_t n = 0;
    n += print(p, FPSTR(str_id), String(aid));
    n += print(p, FPSTR(str_mac), StrUtils::prettyMac(mac));
    n += println(p);
    return n;
}

inline size_t print_network(Print *p, IPAddress ipaddr, const IPAddress subnet,
                            const IPAddress gateway) {
    size_t n = 0;
    n += print(p, FPSTR(str_ip), ipaddr.toString());
    n += print(p, FPSTR(str_subnet), subnet.toString());
    n += print(p, FPSTR(str_gateway), gateway.toString());
    return n;
}

inline size_t print_network(Print *p, IPAddress ipaddr, const IPAddress subnet,
                            const IPAddress gateway, const IPAddress dns) {
    size_t n = 0;
    n += print(p, FPSTR(str_ip), ipaddr.toString());
    n += print(p, FPSTR(str_subnet), subnet.toString());
    n += print(p, FPSTR(str_gateway), gateway.toString());
    n += print(p, FPSTR(str_dns), dns.toString());
    return n;
}

inline size_t print_welcome(Print *p, const char *title, const char *message, const char *footer) {
    size_t n = 0;
    size_t len = SCREEN_WIDTH / 2;
    char tmp[len + 1];

    strcpy(tmp, title);
    strpadd(tmp, StrUtils::CENTER, len, '#');
    n += print(p, tmp);
    n += println(p);

    strcpy(tmp, message);
    strpadd(tmp, StrUtils::CENTER, len);
    n += print(p, tmp);
    n += println(p);

    strcpy(tmp, footer);
    strpadd(tmp, StrUtils::CENTER, len, '#');
    n += print(p, tmp);
    n += println(p);

    return n;
}

}  // namespace PrintUtils
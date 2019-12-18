#pragma once

#include "Arduino.h"

#include "Strings.h"
#include "StrUtils.h"

namespace PrintUtils {

inline size_t print(Print *p, const __FlashStringHelper *ifsh) {
    size_t n = 0;
    n += p->print(ifsh);
    n += p->print(' ');
    return n;
}

template <typename T>
inline size_t print_quoted(Print *p, T value) {
    size_t n = 0;
    n += p->print('\'');
    n += p->print(value);
    n += p->print('\'');
    n += p->print(' ');
    return n;
}

template <typename T>
inline size_t print_ident(Print *p, T value)  {
    size_t n = 0;
    n += p->print('[');
    n += p->print(value);
    n += p->print(']');
    n += p->print(' ');
    return n;
}

template <typename T>
size_t print(Print *p, T v) {
    size_t n = 0;
    n += p->print(v);
    n += p->print(' ');
    return 0;
}

template <typename T, typename... Args>
size_t print(Print *p, T first, Args...args) {
    size_t n = 0;
    n += p->print(first);
    n += p->print(' ');
    n += print(p, args...);
    return n;
}

inline size_t println(Print *p) {
    return p->println();
}

inline size_t println(Print* p, const __FlashStringHelper *ifsh) {
    size_t n = print(p, ifsh);
    n += p->println();
    return n;
}

inline size_t print_not_found(Print *p, const String &str) {
    size_t n = 0;
    n += print_quoted(p, str.c_str());
    n += print(p, FPSTR(str_not));
    n += p->println(FPSTR(str_found));
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
    n += print_quoted(p, str.c_str());
    return n;
}

inline size_t println_unknown_param(Print *p, const String &name) {
    size_t n = 0;
    n += print_unknown(p, name);
    n +=  p->println(FPSTR(str_param));
    return n;
}

inline size_t println_unknown_module(Print *p, const String &name) {
    size_t n = 0;
    n += print_unknown(p, name);
    n += p->println(FPSTR(str_module));
    return n;
}

inline size_t println_unknown_action(Print *p, const String &name) {
    size_t n = 0;
    n += print_unknown(p, name);
    n += p->println(FPSTR(str_action));
    return n;
}

void print_welcome(Print *p, const char *title, const char *message,
                   const char *footer);

void print_delay(Print *p, const char *message, uint8_t wait_s);

inline size_t println_done(Print *p) { return p->println(FPSTR(str_done)); }

inline size_t print_wifi_ap_station(Print *p, const uint8_t aid, const uint8_t *mac, const bool connected) {
    size_t n = 0;
    n += print_ident(p, FPSTR(str_wifi_ap));
    PGM_P str = connected ? str_connected : str_disconnected;
    n += print(p, FPSTR(str));
    n += print(p, FPSTR(str_station));
    n += print(p, FPSTR(str_id), aid);   
    n += print(p, FPSTR(str_mac));
    n += p->println(StrUtils::prettyMac(mac));
    return n;
}

inline size_t print_wifi_ap_station(Print *p, const uint8_t aid, const uint8_t *mac) {
    size_t n = 0;
    n += print(p, FPSTR(str_id), aid);
    n += print(p, FPSTR(str_mac));
    n += p->println(StrUtils::prettyMac(mac));
    return n;
}

inline void print_welcome(Print *p, const char *title, const char *message, const char *footer) {
    size_t width = SCREEN_WIDTH / 2;
    char tmp[width + 1];

    strcpy(tmp, title);
    strpadd(tmp, StrUtils::CENTER, width, '#');
    p->println(tmp);

    strcpy(tmp, message);
    strpadd(tmp, StrUtils::CENTER, width);
    p->println(tmp);

    strcpy(tmp, footer);
    strpadd(tmp, StrUtils::CENTER, width, '#');
    p->println(tmp);
}

}  // namespace PrintUtils
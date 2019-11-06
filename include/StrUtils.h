#pragma once

#include <Arduino.h>
#include <IPAddress.h>
#include <user_interface.h>

#include "Consts.h"

namespace StrUtils {

typedef enum { LEFT, CENTER, RIGHT } Align;

String asJsonObj(const char *key, const char *value);

String asJsonObj(const char *key, String value);

String getTimeStr(const unsigned long epoch_s, bool fmtLong = false);

String getDateTimeStr(const unsigned long epoch_s);

void stringToBytes(const char *str, char sep, uint8_t *bytes, int len,
                   int base);

String iptos(IPAddress &ip);

const char *iptoa(IPAddress &ip);

IPAddress atoip(const char *);

void setnnstr(char *dest, const char *src);

bool setstr(char *dest, const char *src, size_t size);

void strfill(char *str, char chr, size_t size);

bool isip(const char *);

bool isip(const String &);

bool strpositiv(String &);

bool strnegativ(String &);

String fmt_ip_port(const IPAddress &ip, const uint16_t port);

String fmt_ip_port(const char *ip, const uint16_t port);

String fmt_network(const IPAddress ipddr, const IPAddress subnet,
                   const IPAddress gateway);

String fmt_network(const IPAddress ipaddr, const IPAddress subnet,
                   const IPAddress gateway, const IPAddress dns);

String fmt_size(size_t bytes);

String fmt_mhz(uint32_t);

String fmt_mac(const uint8_t *);

String fmt_rssi(int db);

void strpadd(char *str, Align align, size_t size, const char ch = ' ');

String getStr(String &str);

String getStr(IPAddress &value);

String getStr(int num);

String getStrP(PGM_P strP, bool space = true);

String getBoolStr(bool value, bool space = true);

String getOnOffStr(bool value, bool space = true);

String getEnabledStr(bool value, bool space = true);

String getUpDownStr(bool value);

char *getUpDownStr(char *buf, bool value, bool space = true);

String getIdentStr(String &str, bool with_space = true);

String getIdentStr(const char *str, bool with_space);

String getIdentStr(const char *str, bool with_space, char ch);

String getIdentStr(const char *str, bool with_space, char left, char right);

String getIdentStrP(PGM_P strP, bool with_space = true);

String getQuotedStrP(PGM_P str, bool with_space, char ch = '\'');

String getQuotedStr(const char *str, bool with_space = true, char ch = '\'');

String getQuotedStr(String &str, bool with_space = true);

bool atomac(const char *txt, uint8_t *addr);

int hex2num(char c);

int hex2byte(const char *hex);

template <typename T> T concat(T v) { return v; }

template <typename T, typename... Args> String concat(T first, Args... args) {
    return first + " " + concat(args...);
}

} // namespace StrUtils

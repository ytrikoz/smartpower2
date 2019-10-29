#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include "Consts.h"

namespace StrUtils {

typedef enum { LEFT, CENTER, RIGHT } Align;

String getTimeStr(unsigned long time_s, bool longFormat = false);

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

String getSocketStr(IPAddress ip, int port);

String getMacStr(uint8 hwaddr[6]);

String formatSize(size_t bytes);

String formatInMHz(uint32_t);

void strpadd(char *str, Align align, size_t size, const char ch = ' ');

String getStr(String &str);

String getStr(IPAddress &value);

String getStr(long unsigned int value);

String getStr(int num);

String getStrsP(PGM_P...);

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

template <typename T> T concat(T v) { return v; }

template <typename T, typename... Args> String concat(T first, Args... args) {
    return first + " " + concat(args...);
}

} // namespace StrUtils

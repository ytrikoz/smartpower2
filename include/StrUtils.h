#pragma once

#include <Arduino.h>
#include <IPAddress.h>
#include "Strings.h"

namespace StrUtils {

typedef enum { LEFT, CENTER, RIGHT } Align;

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

}  // namespace StrUtils

#pragma once

#include <Arduino.h>
#include <IPAddress.h>
#include "consts.h"

namespace str_utils {

typedef enum { LEFT, CENTER, RIGHT } Align;

void stringToBytes(const char *str, char sep, uint8_t *bytes, int len, int base);
String iptoa(IPAddress);
IPAddress atoip(const char *);
bool setstr(char *dest, const char *src, uint8_t size);
void strfill(char *str, char chr, uint8_t size);

bool isValidIp(const char *);
bool isMeanYes(String &);
bool isMeanNo(String &);

String getSocketStr(IPAddress ip, int port);
String getMacStr(uint8 hwaddr[6]);

String formatSize(size_t bytes);
String formatInMHz(uint32_t);

void strwithpad(char *str, Align align, uint8_t line_width,
                  const char ch = ' ');

}  // namespace str_utils

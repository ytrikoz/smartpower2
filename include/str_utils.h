#pragma once
#include <Arduino.h>

#include "consts.h"

namespace str_utils {

typedef enum { LEFT, CENTER, RIGHT } Align;
String formatSize(size_t bytes);
bool setstr(char *_dest, const char *_src, uint8_t _size);
String strof(int number, char character);

String mac2str(uint8 hwaddr[6]);
void printWelcomeTo(Print *p);

String getStrInMHz(uint32_t freq);
void setStrOfChar(char *str, char chr, uint8_t size);
void addPaddingTo(char *str, Align align, uint8_t line_width,
                  const char ch = ' ');
void stringToBytes(const char *str, char sep, byte *bytes, int len,
                   int base);
}  // namespace str_utils

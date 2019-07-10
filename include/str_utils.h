#pragma once
#include <Arduino.h>

#include "consts.h"

typedef enum { ALIGN_LEFT, ALIGN_CENTER, ALIGN_RIGHT } TextAligment;

int quadraticRegression(double volt);
String formatMHz(uint32_t uint32_t);
String formatSize(size_t bytes);
bool setstr(char *_dest, const char *_src, uint8_t _size);
String strof(int number, char character);
void charsOf(char *str, char chr, uint8_t size);
String mac2str(uint8 hwaddr[6]);
void printWelcome(Print *p);
String marginStr(const char *str, TextAligment align, uint8_t width);
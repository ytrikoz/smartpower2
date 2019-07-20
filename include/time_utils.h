#pragma once

#include <pgmspace.h>
#include "time.h"
#include "Types.h"

uint8_t getDaysInMonth(uint8_t month, uint16_t year);
uint8_t getMonthNum(String str);
bool isLeapYear(uint8_t year);
bool isValidYear(int year);
bool isValidMonth(uint8_t n);
bool isValidHour(uint8_t hour);
bool isValidMinute(uint8_t minute);
bool isValidSecond(uint8_t second);

bool decodeDateStr(char *str, struct tm &tm);
bool decodeTimeStr(char *str, struct tm &tm);

long timePassed(unsigned long start_ms, unsigned long finish_ms);

long millisSince(unsigned long since_ms);

void encodeTime(unsigned long epoch_s, struct tm &tm);

unsigned long getAppBuildTime();
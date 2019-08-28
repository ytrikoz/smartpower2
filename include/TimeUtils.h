#pragma once

#include <Arduino.h>
#include <time.h>

unsigned long getAppBuildEpoch();
unsigned long millis_since(unsigned long since);
unsigned long millis_passed(unsigned long start, unsigned long finish);

bool isLeapYear(uint16_t year);
uint8_t getDaysInMonth(uint8_t month, uint16_t year);
uint16_t encodeYear(uint16_t& year);
uint8_t encodeMonth(String str);
bool encodeDate(uint8_t mday, uint8_t month, int16_t year);
bool encodeDate(char *str, struct tm &tm);
bool encodeTime(char *str, struct tm &tm);
void epochToDateTime(unsigned long epoch_s, struct tm &tm);
bool isValidMonth(uint8_t month);
bool isValidHour(uint8_t hour);
bool isValidMinute(uint8_t minute);
bool isValidSecond(uint8_t second);
uint32_t encodeEpoch(tm tm);
uint32_t encodeEpochTime(uint8_t hour, uint8_t minute, uint8_t second);
uint32_t encodeEpochDate(uint16_t year, uint8_t month, uint16_t day);
size_t tmtodtf(struct tm &tm, char *str);

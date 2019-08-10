#pragma once

#include <pgmspace.h>
#include <time.h>

#include "Consts.h"
#include "StrUtils.h"
#include "BuildConfig.h"

uint8_t get_days_in_month(uint8_t month, uint16_t year);
uint8_t str_to_month(String str);

bool is_leap_year(uint16_t year);
bool is_valid_date(int day, int month, int year);
bool is_valid_year(uint16_t n);
bool is_valid_month(uint8_t n);
bool is_valid_hour(uint8_t n);
bool is_valid_minute(uint8_t n);
bool is_valid_seconds(uint8_t n);
bool is_valid_day(uint8_t n);

bool str_to_date(char *str, struct tm &tm);
bool str_to_time(char *str, struct tm &tm);

unsigned long millis_since(unsigned long since);
unsigned long millis_passed(unsigned long start, unsigned long finish);

void epoch_to_tm(unsigned long epoch_s, struct tm &tm);

unsigned long get_appbuild_epoch();

uint32_t get_epoch(tm dateTime);

uint32_t get_epoch(int year, int month, int day, int hour, int minute,
                   int second);

char *tmtoa(struct tm *tm, char *buf);

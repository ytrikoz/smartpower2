#pragma once
#include <pgmspace.h>
#include "Types.h"

typedef struct {
    char name[4];
    uint8_t days;
} Month;

static const Month months[] PROGMEM = {{"Jan", 31}, {"Feb", 28}, {"Mar", 31},
                                       {"Apr", 30}, {"May", 31}, {"Jun", 30},
                                       {"Jul", 31}, {"Aug", 31}, {"Sep", 30},
                                       {"Oct", 31}, {"Nov", 30}, {"Dec", 31}};

static unsigned long toEpochTime(uint16_t days, uint8_t h, uint8_t m,
                                 uint8_t s) {
    return ((days * 24L + h) * 60 + m) * 60 + s;
}

static uint8_t getMonthNumber(const char *str)
{
    String s = String(str);
    for(uint8_t n = 0; n < 11; n++)
        if (s.equalsIgnoreCase(months[i].name) == 1)
}
static unsigned long decodeDateStr(const char*, &DateTime dt)
{
    
//Jul 18 2019

}



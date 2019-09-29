#pragma once

#include "Strings.h"

static const char *strP_module[APP_MODULES] PROGMEM = {
    str_btn, str_clock, str_http,  str_lcd,    str_led,   str_netsvc,
    str_ntp, str_psu,   str_shell, str_telnet, str_update};

inline char *boolStr(char *buf, bool value) {
    return strcpy_P(buf, value ? str_true : str_false);
}

inline char *moduleStr(char *buf, uint8_t index) {
    PGM_P strP = (char *)pgm_read_ptr(&(strP_module[index]));
    strcpy_P(buf, strP);
    return buf;
}

inline String getModuleName(uint8_t index) {
    PGM_P strP = (char *)pgm_read_ptr(&(strP_module[index]));
    char buf[64];
    strcpy_P(buf, strP);
    return String(buf);
}

inline char *getModuleName(uint8_t index, char *buf, size_t &size) {
    PGM_P strP = (char *)pgm_read_ptr(&(strP_module[index]));
    strcpy_P(buf, strP);
    size = strlen(buf);
    return buf;
}

inline char *paramStrP(char *buf, PGM_P strP) {
    buf = strcpy_P(buf, strP);
    size_t len = strlen(buf);
    buf[len] = ':';
    buf[++len] = ' ';
    buf[++len] = '\x00';
    return buf;
}
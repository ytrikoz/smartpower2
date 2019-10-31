#pragma once

#include "Config.h"
#include "Strings.h"

namespace AppUtils {

static const char *strP_module[APP_MODULES] PROGMEM = {
    str_btn, str_clock, str_http,  str_display, str_led,    str_netsvc,
    str_ntp, str_psu,   str_shell, str_telnet,  str_update, str_syslog};

inline String boolStr(bool value) {
    return String(value ? FPSTR(str_true) : FPSTR(str_false));
}

inline char *moduleStr(char *buf, uint8_t index) {
    PGM_P strP = (char *)pgm_read_ptr(&(strP_module[index]));
    strcpy_P(buf, strP);
    return buf;
}

inline String getModuleName(uint8_t index) {
    PGM_P strP = (char *)pgm_read_ptr(&(strP_module[index]));
    char buf[16];
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
    buf[len++] = ':';
    buf[len++] = ' ';
    buf[len++] = '\x00';
    return buf;
}

inline String getNetworkConfig(Config *cfg) {
    String res = String(SET_NETWORK);
    res += cfg->getValueAsByte(WIFI);
    res += ',';
    res += cfg->getValueAsString(SSID);
    res += ',';
    res += cfg->getValueAsString(PASSWORD);
    res += ',';
    res += cfg->getValueAsBool(DHCP);
    res += ',';
    res += cfg->getValueAsString(IPADDR);
    res += ',';
    res += cfg->getValueAsString(NETMASK);
    res += ',';
    res += cfg->getValueAsString(GATEWAY);
    res += ',';
    res += cfg->getValueAsString(DNS);
    return res;
}
} // namespace AppUtils
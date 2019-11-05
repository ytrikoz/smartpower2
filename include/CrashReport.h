#pragma once

#include <Arduino.h>
#include <FS.h>

struct AppCrash {
    uint32 reason;
    uint32 exccause;
    uint32 epc1;
    uint32 epc2;
    uint32 epc3;
    uint32 excvaddr;
    uint32 depc;
    uint32 stack_start;
    uint32 stack_end;
};

class CrashReport {
  public:
    CrashReport();
    bool print(Print *p, const char *name);
    bool print(Print *p, File &f);

  private:
    String getName(uint8_t num, unsigned long uptime);
};

static const char str_crash_report[] PROGMEM = ">>>crash>>>";

static const char str_restart0[] PROGMEM = "DEFAULT";
static const char str_restart1[] PROGMEM = "WDT";
static const char str_restart2[] PROGMEM = "EXCEPTION";
static const char str_restart3[] PROGMEM = "SOFT_WDT";
static const char str_restart4[] PROGMEM = "SOFT_RESTART";
static const char str_restart5[] PROGMEM = "DEEP_SLEEP";
static const char str_restart6[] PROGMEM = "EXT_SYS";

static const char strf_restart_reason[] PROGMEM = "Restart (%d): %s\r\n";

static const char strf_except_cause[] PROGMEM = "Exception (%d): %s\r\n";

static const char str_cause_unlisted[] PROGMEM = "unlisted";
static const char str_cause_reserved[] PROGMEM = "reserved";

static const char str_except0[] PROGMEM = "Invalid command";
static const char str_except1[] PROGMEM = "Syscall instruction";
static const char str_except2[] PROGMEM = "Instruction Fetch Error";
static const char str_except3[] PROGMEM = "Load Store Error";
static const char str_except4[] PROGMEM = "Level 1 Interrupt";
static const char str_except5[] PROGMEM = "Alloca Cause";
static const char str_except6[] PROGMEM = "Division by zero";
static const char str_except8[] PROGMEM = "Privileged Cause";
static const char str_except9[] PROGMEM =
    "Unaligned read/write operation addresses";

static const char strf_except[] PROGMEM = "epc1=0x%08x epc2=0x%08x epc3=0x%08x "
                                          "excvaddr=0x%08x depc=0x%08x\r\n";

static const char str_stack[] PROGMEM = "\r\n>>>stack>>>\r\n\r\n";

static const char *strP_restart[] PROGMEM = {
    str_restart0, str_restart1, str_restart2, str_restart3,
    str_restart4, str_restart5, str_restart6};

static const char *strP_causes[] PROGMEM = {
    str_except0, str_except1,        str_except2, str_except3, str_except5,
    str_except6, str_cause_reserved, str_except8, str_except9};

inline String getExceptStr(uint8_t num) {
    char buf[64];
    PGM_P strP = str_cause_unlisted;
    if (num >= 0 && num <= 9)
        strP = (char *)pgm_read_ptr(&(strP_causes[num]));
    strcpy_P(buf, strP);
    return String(buf);
}

inline String getRestartStr(uint8_t num) {
    char buf[64];
    PGM_P strP = str_cause_unlisted;
    if (num >= 0 && num <= 6)
        strP = (char *)pgm_read_ptr(&(strP_restart[num]));
    strcpy_P(buf, strP);
    return String(buf);
}

#pragma once

#include "Arduino.h"

static const char str_crash_report[] PROGMEM = "crash report";
static const char str_restart_reason[] PROGMEM = "restart reason";
static const char str_exception[] PROGMEM = "Exception";
static const char str_stack[] PROGMEM = ">>>stack>>>";
static const char strf_exception[] PROGMEM =
    "epc1=0x%08x epc2=0x%08x epc3=0x%08x "
    "excvaddr=0x%08x depc=0x%08x";

struct AppCrash {
    uint32 reason;
    uint32 exccause;
    uint32 epc1;
    uint32 epc2;
    uint32 epc3;
    uint32 excvaddr;
    uint32 depc;
    uint32 stack_size;
};

class CrashReport {
  public:
    CrashReport();
    bool load(String &name);
    bool load(const char *name);
    void write(AppCrash c);
    void print(Print *p);
    uint8_t getNum();

  private:
    uint8_t num;
    AppCrash cur;
    String getName(uint8_t num, unsigned long uptime);
};
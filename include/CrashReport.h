#pragma once

#include "Arduino.h"

static const char str_crash_report[] PROGMEM = "crash report";
static const char str_restart_reason[] PROGMEM = "restart reason";
static const char str_exception_cause[] PROGMEM = "exception cause";

class CrashReport {
  public:
    bool print(Print *p, const char *name);
};
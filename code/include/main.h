#include <Arduino.h>

#include <user_interface.h>

#include <SimpleCLI.h>

#include "Consts.h"
#include "App.h"
#include "Logger.h"
#include "LoopWatcher.h"
#include "BootWatcher.h"
#include "CrashReport.h"

extern BootWatcher boot;
extern LoopWatcher* loopWatcher;
extern SimpleCLI* cli;
extern Logger Log;
extern App app;

extern bool crashReportEnabled;
extern uint8_t crashReportNumber;
extern void initCrashReport();
extern "C" void custom_crash_callback(struct rst_info* rst_info,
                                      uint32_t stack_start,
                                      uint32_t stack_end);
void preinit();
void setup();
void loop();

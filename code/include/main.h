#include <Arduino.h>

#include <user_interface.h>

extern bool crashReportEnabled_;

extern uint8_t crashReportNumber_;

extern void initCrashReport();

extern "C" void custom_crash_callback(struct rst_info* rst_info,
                                      uint32_t stack_start,
                                      uint32_t stack_end);
void preinit();

void setup();

void loop();

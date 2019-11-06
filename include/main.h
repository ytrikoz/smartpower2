#include <Arduino.h>
#include <user_interface.h>

void preinit(void);
void setup(void);
void loop(void);

bool hasBootError();
void beforeStart();
void afterStart();
void clearBootError();

void initCrashReport();
uint8_t getCrashReportNum();
bool isCrashReportEnabled();
String getCrashName();

void initSerial();
bool initFS();
void initApp();

extern "C" void custom_crash_callback(struct rst_info *rst_info,
                                      uint32_t stack_start, uint32_t stack_end);
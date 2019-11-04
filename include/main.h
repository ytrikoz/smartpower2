#include <Arduino.h>
#include <user_interface.h>

void setup(void);
void loop(void);

extern "C" void custom_crash_callback(struct rst_info *rst_info, uint32_t stack,
                                      uint32_t stack_end);
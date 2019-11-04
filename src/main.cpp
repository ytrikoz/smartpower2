#include "main.h"

#include <Arduino.h>
#include <FS.h>

#include "App.h"
#include "BuildConfig.h"

extern "C" void custom_crash_callback(struct rst_info *rst_info, uint32_t stack,
                                      uint32_t stack_end) {
    unsigned long time = millis();
    char name[32];
    strcpy(name, CRASH_ROOT);
    char tmp[16];
    strcat(name, itoa(time, tmp, DEC));
    File file = SPIFFS.open(name, "w");
    file.println(rst_info->reason);
    file.println(rst_info->exccause);
    file.println(rst_info->epc1);
    file.println(rst_info->epc2);
    file.println(rst_info->epc3);
    file.println(rst_info->excvaddr);
    file.println(rst_info->depc);
    file.println(stack);
    file.println(stack_end);
    for (uint32_t addr = stack; addr < stack_end; ++addr) {
        byte *value = (byte *)addr;
        file.print(*value);
    }
    file.println();
    file.flush();
    file.close();
}

void setup() {
    // Setup serial
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();

    SPIFFS.begin();

#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif
    // Try pushing frequency to 160MHz.
    // system_update_cpu_freq(SYS_CPU_160MHZ);
    system_update_cpu_freq(SYS_CPU_80MHZ);
    app.init(&USE_SERIAL);
    app.start();
}

void loop() { app.loop(); }

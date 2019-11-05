#include "main.h"

#include <Arduino.h>
#include <FS.h>

#include "App.h"
#include "BuildConfig.h"
#include "CrashReport.h"

CrashReport crash;

extern "C" void custom_crash_callback(struct rst_info *rst_info,
                                      uint32_t stack_start,
                                      uint32_t stack_end) {
    SPIFFS.begin();
    String name(FS_CRASH_ROOT);
    name += String(millis());

    AppCrash c;
    c.reason = rst_info->reason;
    c.exccause = rst_info->exccause;
    c.epc1 = rst_info->epc1;
    c.epc2 = rst_info->epc2;
    c.epc3 = rst_info->epc3;
    c.excvaddr = rst_info->excvaddr;
    c.depc = rst_info->depc;
    c.stack_size = stack_end - stack_start;

    File f = SPIFFS.open(name, "w");
    f.write((byte *)&c, sizeof(c));
    f.flush();
    f.close();

    /*
        file.println(stack);
        file.println(stack_end);
        for (uint32_t addr = stack; addr < stack_end; ++addr) {
            byte *value = (byte *)addr;
            file.print(*value);
        }

    file.println();
    file.flush();
    file.close();
            */
}

void setup() {
    // Setup serial
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();

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

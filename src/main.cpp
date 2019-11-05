#include "main.h"

#include <Arduino.h>
#include <FS.h>

#include "App.h"
#include "BuildConfig.h"
#include "CrashReport.h"
#include "FSUtils.h"
#include "StrUtils.h"

uint8_t crashNum = 0;

extern "C" void custom_crash_callback(struct rst_info *rst_info,
                                      uint32_t stack_start,
                                      uint32_t stack_end) {

    if (!isCrashReportEnabled())
        return;

    String name = getCrashName();
    File f = SPIFFS.open(name, "w");
    if (f) {
        AppCrash c;
        c.reason = rst_info->reason;
        c.exccause = rst_info->exccause;
        c.epc1 = rst_info->epc1;
        c.epc2 = rst_info->epc2;
        c.epc3 = rst_info->epc3;
        c.excvaddr = rst_info->excvaddr;
        c.depc = rst_info->depc;
        c.stack_start = stack_start;
        c.stack_end = stack_end;
        size_t file_size = f.write((byte *)&c, sizeof(c));
        for (uint32_t addr = stack_start; addr < stack_end; ++addr) {
            uint8_t *value = (uint8_t *)addr;
            file_size += f.write(*value);
            if (file_size >= CRASH_MAX_SIZE - 2)
                break;
        }
        f.println();
        f.flush();
        f.close();
    }
}

String getCrashName() {
    char buf[32];
    sprintf(buf, "%s%d_%lu", FS_CRASH_ROOT, crashNum + 1, millis());
    return String(buf);
}

bool isCrashReportEnabled() { return crashNum < CRASH_MAX_NUM; }

void setup() {
    // Setup serial
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();

    SPIFFS.begin();
    crashNum = getFilesCount(FS_CRASH_ROOT);

    USE_SERIAL.print(StrUtils::getIdentStrP(str_crash));
    if (isCrashReportEnabled()) {
        USE_SERIAL.println(crashNum);
    } else {
        USE_SERIAL.println(StrUtils::getStrP(str_disabled));
    }

#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif
    // Try pushing frequency to 160MHz.
    system_update_cpu_freq(SYS_CPU_160MHZ);
    // system_update_cpu_freq(SYS_CPU_80MHZ);
    app.init(&USE_SERIAL);
    app.start();
}

void loop() { app.loop(); }

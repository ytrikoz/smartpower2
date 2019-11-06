#include "main.h"

#include <Arduino.h>
#include <FS.h>

#include "App.h"
#include "BuildConfig.h"
#include "CrashReport.h"
#include "FSUtils.h"
#include "StrUtils.h"

bool crashEnabled = false;
uint8_t crashNum = 0;
bool safeBoot = false;
bool setupDone = false;

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

void preinit() {
    // SYS_CPU_80MHZ
    system_update_cpu_freq(SYS_CPU_160MHZ);
}

void setup() {
    initSerial();

    if (!initFS()) {
        USE_SERIAL.println(F("[FS] error"));
        return;
    }

    initCrashReport();

    if (getCrashReportNum()) {
        USE_SERIAL.print(getIdentStrP(str_crash));
        USE_SERIAL.print(getStrP(str_stored));
        USE_SERIAL.print(' ');
        USE_SERIAL.println(crashNum);
    }

    if (!isCrashReportEnabled()) {
        USE_SERIAL.print(getIdentStrP(str_crash));
        USE_SERIAL.println(FPSTR(str_disabled));
    }

    initApp();

    safeBoot = hasBootError();
    if (safeBoot) {
        USE_SERIAL.print(getIdentStrP(str_boot));
        USE_SERIAL.println(FPSTR(str_safe));
        clearBootError();
        app.startSafe();
    } else {
        beforeStart();
        app.start();
        afterStart();
    }
    setupDone = true;
}

void loop() {
    if (!setupDone)
        return;

    if (safeBoot)
        app.loopSafe();
    else
        app.loop();
}

bool hasBootError() {
    bool res = SPIFFS.exists(FS_START_FLAG);
    return res;
}

void beforeStart() {
    File f = SPIFFS.open(FS_START_FLAG, "w");
    f.print(APP_VERSION);
    f.flush();
    f.close();
}

void afterStart() { SPIFFS.remove(FS_START_FLAG); }

void clearBootError() { SPIFFS.remove(FS_START_FLAG); }

void initSerial() {
    USE_SERIAL.begin(115200);
    USE_SERIAL.println();
#ifdef SERIAL_DEBUG
    USE_SERIAL.setDebugOutput(true);
#endif
}

bool initFS() { return SPIFFS.begin(); }

void initApp() { app.init(&USE_SERIAL); }

/*
 * CRASH REPORT
 */
void initCrashReport() {
    crashNum = getFilesCount(FS_CRASH_ROOT);
    crashEnabled = crashNum < CRASH_MAX_NUM;
}

bool isCrashReportEnabled() { return crashEnabled; }

uint8_t getCrashReportNum() { return crashNum; };

String getCrashName() {
    char buf[32];
    sprintf(buf, "%s%d_%lu", FS_CRASH_ROOT, crashNum + 1, millis());
    return String(buf);
}
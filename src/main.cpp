#include "main.h"

#include <Arduino.h>
#include <FS.h>

#include "App.h"
#include "BootWatcher.h"
#include "BuildConfig.h"
#include "CrashReport.h"
#include "FSUtils.h"
#include "PrintUtils.h"
#include "StrUtils.h"

using namespace PrintUtils;

bool crashEnabled = false;
uint8_t crashNum = 0;

bool setupDone;

BootWatcher *bw;

extern "C" void custom_crash_callback(struct rst_info *rst_info,
                                      uint32_t stack_start,
                                      uint32_t stack_end) {
    if (!isCrashReportEnabled())
        return;

    String name = getCrashName();
    if (File f = SPIFFS.open(name, "w")) {
        CrashHeader h;
        h.reason = rst_info->reason;
        h.exccause = rst_info->exccause;
        h.epc1 = rst_info->epc1;
        h.epc2 = rst_info->epc2;
        h.epc3 = rst_info->epc3;
        h.excvaddr = rst_info->excvaddr;
        h.depc = rst_info->depc;
        h.stack_start = stack_start;
        h.stack_end = stack_end;
        size_t file_size = f.write((byte *)&h, sizeof(h));
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
    system_update_cpu_freq(SYS_CPU_160MHZ);
    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    setupDone = false;
}

void setup() {
    initSerial();

    initWire();

    if (!initFS()) {
        println_moduleP_nameP_value(&Serial, str_spiffs, str_error, 0);
        return;
    };

    bw = new BootWatcher();

    initCrashReport();

    if (getCrashReportNum()) {
        INFO.print(getIdentStrP(str_crash));
        INFO.print(getStrP(str_stored));
        INFO.print(' ');
        INFO.println(crashNum);
    }

    if (!isCrashReportEnabled()) {
        INFO.print(getIdentStrP(str_crash));
        INFO.println(FPSTR(str_disabled));
    }

    initApp();
    
    if (bw->isSafeBootMode()) {
        INFO.setDebugOutput(true);
        INFO.print(getIdentStrP(str_boot));
        INFO.println(FPSTR(str_safe));
        app.startSafe();
    } else {
        beforeStart();
        app.start();
    }
    afterStart();
}

void loop() {
    if (bw->isSafeBootMode())
        app.loopSafe();
    else
        app.loop();
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
    INFO.begin(115200);
    INFO.println();
    print_welcome(&INFO, " Welcome ", APP_NAME " v" APP_VERSION,
                  " " BUILD_DATE " ");
}

void initWire() { Wire.begin(I2C_SDA, I2C_SCL); }

bool initFS() { return SPIFFS.begin(); }

void initApp() { app.init(&INFO); }

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
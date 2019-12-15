#include "main.h"

#include <Arduino.h>
#include <FS.h>

#include "BuildConfig.h"
#include "CrashReport.h"
#include "FSUtils.h"
#include "PrintUtils.h"
#include "StrUtils.h"

using namespace PrintUtils;

BootWatcher boot;
Logger logger;
App app;

bool crashReportEnabled = false;
uint8_t crashReportNumber = 0;


extern "C" void custom_crash_callback(struct rst_info *rst_info,
                                      uint32_t stack_start,
                                      uint32_t stack_end) {
    if (!crashReportEnabled) return;
    char buf[32];
    sprintf(buf, "%s%d_%lu", CRASH_ROOT, crashReportNumber + 1, millis());
    if (File f = SPIFFS.open(buf, "w")) {
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
            if (file_size >= CRASH_SIZE - 2)
                break;
        }
        f.println();
        f.flush();
        f.close();
    }
}

void initCrashReport() {
    crashReportNumber = FSUtils::getFilesCount(CRASH_ROOT);
    crashReportEnabled = crashReportNumber < CRASH_NUM;

    if (crashReportNumber) {
        PrintUtils::print_ident(&Serial, FPSTR(str_crash));
        PrintUtils::print(&Serial, crashReportNumber);

        if (!crashReportEnabled) PrintUtils::print(&Serial, FPSTR(str_disabled));

        PrintUtils::print_ln(&Serial);
    }
}

bool setupDone;
void preinit() {
    system_update_cpu_freq(SYS_CPU_160MHZ);
    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    wifi_station_set_hostname(APP_NAME);
    setupDone = false;
}

void setup() {
    boot.init();
    
    initCrashReport();

    PrintUtils::println(&logger, "[log] start");


    app.setOutput(&logger);    
    app.init();

    if (boot.isSafeMode()) {
        INFO.setDebugOutput(true);
        INFO.print(getIdentStrP(str_boot));
        INFO.println(FPSTR(str_safe));
        app.startSafe();
    } else {
        app.begin();
    }

    boot.end();

    setupDone = true;
}

void loop() {
    if (!setupDone) return;
    logger.loop();
    if (boot.isSafeMode())
        app.loopSafe();
    else {
        app.loop();
    }

}

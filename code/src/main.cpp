#include "main.h"

#include <Arduino.h>
#include <FS.h>

#include "Global.h"
#include "BuildConfig.h"
#include "CrashReport.h"
#include "LoopTiming.h"
#include "Utils/FSUtils.h"
#include "Utils/PrintUtils.h"
#include "Utils/StrUtils.h"

bool crashReportEnabled_ = false;
uint8_t crashReportNumber_ = 0;
bool setupDone = false;

void handleState(AppState state) {
    if (state == STATE_RESET) {
        PrintUtils::print_ident(&Serial, FPSTR(str_config));
        PrintUtils::print(&Serial, FPSTR(str_reset));
        config->setDefaultParams();
        config->save();
    }        
    if (state >= STATE_RESTART) {
        system_restart();
        delay(100);
    }
}

void preinit() {
    system_update_cpu_freq(SYS_CPU_160MHZ);
    WiFi.persistent(false);
    WiFi.setAutoConnect(false);
    wifi_station_set_hostname(APP_NAME);
}

void setup() {
    boot.setOutput(&syslog);
    boot.init();
    
    initCrashReport();
    
    boot.start();

    config = new ConfigHelper(FS_MAIN_CONFIG);
    config->setOutput(&syslog);
    if (!config->check()) {
        PrintUtils::print_ident(&syslog, FPSTR(str_config));
        PrintUtils::print_file_not_found(&syslog, config->name());
        config->setDefaultParams();
        config->save();
    }
    config->load();
    app.setConfig(config);
    app.setOutput(&syslog);    

    powerlog = new PowerLog();
    app.setPowerlog(powerlog);
   
    Cli::init();
    
    app.begin();

    boot.end();

    setupDone = true;
}

void loop() {
    if (!setupDone) {
        return;
    } 
    syslog.loop();

    AppState res = app.loop(loopTimer);
    
    if (loopTimer) loopTimer->tick();
    
    handleState(res);
}

void initCrashReport() {
    crashReportNumber_ = FSUtils::getFilesCount(CRASH_ROOT);
    crashReportEnabled_ = crashReportNumber_ < CRASH_NUM;
    if (crashReportNumber_) {
        PrintUtils::print_ident(&syslog, FPSTR(str_crash));
        PrintUtils::print(&syslog, crashReportNumber_);
        if (!crashReportEnabled_)         
            PrintUtils::print(&syslog, FPSTR(str_off));
         PrintUtils::println(&syslog);
    }
}

extern "C" void custom_crash_callback(struct rst_info *rst_info,
                                      uint32_t stack_start,
                                      uint32_t stack_end) {
    if (!crashReportEnabled_) return;
    char buf[32];
    sprintf(buf, "%s%d_%lu", CRASH_ROOT, crashReportNumber_ + 1, millis());
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

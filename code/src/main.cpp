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
bool setupDone = true;
bool safeMode = false;

ModuleDef mods[MODULES_COUNT] = {
    {0, str_btn, false},
    {0, str_led, false},
    {0, str_clock, false},
    {0, str_psu, false},
    {0, str_display, false},
    {0, str_console, false},
    {0, str_netsvc, true},
    {0, str_telnet, true},
    {0, str_update, true},
    {0, str_syslog, true},
    {0, str_web, true}};

void handleAppState(AppState state) {
    if (state == STATE_FAILBACK) {
        PrintUtils::print_ident(&Serial, FPSTR(str_config));
        PrintUtils::print(&Serial, FPSTR(str_reset));
        config->setDefaultConfig();
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
    booter = new BootWatcher();
    booter->setOutput(&mainlog);
    safeMode = booter->init();
    initCrashReport();

    booter->start();
    config = new ConfigHelper(FS_MAIN_CONFIG);
    config->setOutput(&mainlog);
    if (!config->check()) {
        PrintUtils::print_ident(&mainlog, FPSTR(str_config));
        PrintUtils::print_file_not_found(&mainlog, config->name());
        config->setDefaultConfig();
        config->save();
    }
    config->load();

    wireless = new Wireless();
    wireless->setOutput(&mainlog);
    wireless->start();

    mods[MOD_LED].obj = new Modules ::Led();
    mods[MOD_BTN].obj = new Modules::Button(POWER_BTN_PIN);
    mods[MOD_CLOCK].obj = new Modules::Clock();
    mods[MOD_PSU].obj = new Modules::Psu();
    mods[MOD_DISPLAY].obj = new Modules::Display();
    mods[MOD_CONSOLE].obj = new Modules::Console();
    mods[MOD_NETSVC].obj = new NetworkService();
    mods[MOD_TELNET].obj = new Modules ::Telnet(TELNET_PORT);
    mods[MOD_UPDATE].obj = new Modules::OTAUpdate(OTA_PORT);
    mods[MOD_SYSLOG].obj = new Modules::Syslog();
    mods[MOD_WEB].obj = new Modules::Web();

    app.setConfig(config);
    app.setWireless(wireless);
    app.setModules(mods);
    app.setOutput(&mainlog);

    powerlog = new PowerLog();
    app.setPowerlog(powerlog);

    Cli::init();

    app.begin();
}

void loop() {
    if (!setupDone)
        return;
    AppState res = app.loop(looper);
    if (looper)
        looper->tick();
    if (booter) {
        booter->end();
        delete booter;
        booter = nullptr;
    }
    handleAppState(res);
}

void initCrashReport() {
    crashReportNumber_ = FSUtils::getFilesCount(CRASH_ROOT);
    crashReportEnabled_ = crashReportNumber_ < CRASH_NUM;
    if (crashReportNumber_) {
        PrintUtils::print_ident(&mainlog, FPSTR(str_crash));
        PrintUtils::print(&mainlog, crashReportNumber_);
        if (!crashReportEnabled_)
            PrintUtils::print(&mainlog, FPSTR(str_off));
        PrintUtils::println(&mainlog);
    }
}

extern "C" void custom_crash_callback(struct rst_info *rst_info,
                                      uint32_t stack_start,
                                      uint32_t stack_end) {
    if (!crashReportEnabled_) return;
    char buf[32];
    sprintf(buf, "%s%d_%lu", CRASH_ROOT, crashReportNumber_ + 1, millis());
    if (File f = LittleFS.open(buf, "w")) {
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

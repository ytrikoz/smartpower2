#pragma once

#include <Arduino.h>

#include "ConfigHelper.h"
#include "LoopLogger.h"
#include "Plot.h"
#include "PsuLogger.h"
#include "Strings.h"

#include "Modules/ButtonMod.h"
#include "Modules/Display.h"
#include "Modules/Leds.h"
#include "Modules/NetworkService.h"
#include "Modules/OTAUpdate.h"
#include "Modules/PsuModule.h"
#include "Modules/ShellMod.h"
#include "Modules/SyslogMod.h"
#include "Modules/ClockMod.h"
#include "Modules/TelnetServer.h"
#include "Modules/WebMod.h"

class App : PsuLogger {
   public:
    void log(PsuInfo &item) override;

   public:
    App();
    void restart(time_t delay = 0);
    void handleRestart();
    void init(Print *p);
    void start();
    void startSafe();
    bool start(const AppModuleEnum);
    void stop(const AppModuleEnum);
    void loop();
    void loopSafe();

    size_t printDiag(Print *p);
    size_t printDiag(Print *p, const AppModuleEnum module);
    void printLoopCapture(Print *p);
    void printPlot(PlotData *data, Print *p);
    void printCapture(Print *);

    AppModule *getModuleByName(const char *);
    AppModule *getModule(const AppModuleEnum);

    Config *params();
    ClockMod *clock();
    Display *lcd();
    LoopLogger *getLoopLogger();
    MemoryPsuLogger *getPsuLog();
    ConfigHelper *config();
    WebMod *web();
    Psu *psu();
    LedMod *led();
    ShellMod *shell();
    ButtonMod *btn();
    TelnetServer *telnet();

    bool setBootPowerState(BootPowerState state);
    bool setOutputVoltageAsDefault();
    uint8_t getTPW();
    void refresh_power_led();
    void refresh_wifi_led();

    void logInfo(const String &routine, const String &msg) {
        String message = routine;
        message += " : ";
        message += msg;
        logMessage(LEVEL_INFO, message);
    }

    void logMessage(const LogLevel level, const String &msg) {
        char buf[16];
        out->print(millis() / ONE_SECOND_ms);
        out->print(' ');
        out->print(getLogLevel(buf, level));
        out->print(' ');
        out->println(msg);
        out->flush();
    }

    char *getLogLevel(char *buf, LogLevel level) {
        PGM_P strP;
        switch (level) {
            case LEVEL_ERROR:
                strP = str_error;
                break;
            case LEVEL_WARN:
                strP = str_warn;
                break;
            case LEVEL_INFO:
                strP = str_info;
                break;
            default:
                strP = str_unknown;
                break;
        }
        char str[16];
        strncpy_P(str, strP, 13);
        for (char *p = str; *p; p++)
            *p = toupper(*p);
        sprintf(buf, "[%s]", str);
        return buf;
    }

   private:
    void displayProgress(uint8_t progress, const char *message);
    void restartNetworkDependedModules(Wireless::NetworkMode mode, bool hasNetwork);
    void restart();
    void send_psu_data_to_clients();

   private:
    bool networkChanged;
    bool safemode = false;
    AppModule *appMod[APP_MODULES];
    LoopLogger *loopLogger;
    ConfigHelper *configHelper;

    MemoryPsuLogger *psuLog;

    WiFiEventHandler onDisconnected, onGotIp;

    unsigned long restartUpdated_;
    time_t restartCountdown_;
    bool restartFlag_;

    uint8_t boot_per;
    Print *out, *dbg, *err = NULL;
};

extern App app;
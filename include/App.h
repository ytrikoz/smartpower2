#pragma once

#include <Arduino.h>

#include "ConfigHelper.h"
#include "LoopLogger.h"
#include "Plot.h"
#include "PsuLogger.h"
#include "Strings.h"

#include "Modules/Button.h"
#include "Modules/Display.h"
#include "Modules/HttpMod.h"
#include "Modules/Leds.h"
#include "Modules/NetworkService.h"
#include "Modules/OTAUpdate.h"
#include "Modules/Psu.h"
#include "Modules/ShellMod.h"
#include "Modules/SyslogMod.h"
#include "Modules/ClockMod.h"
#include "Modules/TelnetServer.h"

class App {
   public:
    App();
    void init(Print *p);
    void start();
    void startSafe();
    bool begin(const AppModuleEnum);
    void stop(const AppModuleEnum);
    void loop();
    void loopSafe();
    void restart(uint8_t time);
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
    ConfigHelper *config();
    HttpMod *http();
    Psu *psu();
    LedMod *led();
    ShellMod *shell();
    Button *btn();
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
    void handleRestart();
    void send_psu_data_to_clients();

   private:
    bool networkChanged;
    AppModule *appMod[APP_MODULES];
    LoopLogger *loopLogger;
    ConfigHelper *configHelper;

    PsuLogger *logger;
    bool safemode = false;
    WiFiEventHandler onDisconnected, onGotIp;
    unsigned long lastUpdated;
    uint8_t reboot;
    uint8_t boot_per;

    Print *out, *dbg, *err = NULL;
};

extern App app;
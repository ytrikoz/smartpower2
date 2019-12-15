#pragma once

#include <Arduino.h>

#include "CommonTypes.h"
#include "ConfigHelper.h"
#include "LoopWatcher.h"
#include "Plot.h"
#include "PsuLogHelper.h"

#include "Modules/ModuleHost.h"

#include "Modules/Clock.h"
#include "Modules/Button.h"
#include "Modules/Led.h"
#include "Modules/Display.h"
#include "Modules/NetworkService.h"
#include "Modules/OTAUpdate.h"
#include "Modules/PsuModule.h"
#include "Modules/SyslogModule.h"
#include "Modules/Shell.h"
#include "Modules/Telnet.h"
#include "Modules/Web.h"

class App : public ModuleHost, PsuListener {
   public:
    Module *instance(ModuleEnum) override;
    String name(ModuleEnum) const override;
    bool get(const char *, ModuleEnum &) const override;

   public:
    void log(PsuData &item) override;

   public:
    App();
    void restart(time_t delay = 0);
    void handleRestart();
    void init();
    void begin();
    void startSafe();
    void loop();
    void loopSafe();

    size_t printDiag(Print *p);
    size_t printDiag(Print *p, const ModuleEnum module);
    void printLoopCapture(Print *p);
    void printPlot(PlotData *data, Print *p);
    void printCapture(Print *);

    Modules::Button *btn() { return (Modules::Button *)appMod[MOD_BTN]; }
    Modules::Clock *clock() { return (Modules::Clock *)appMod[MOD_CLOCK]; }    
    Modules::Led *led() { return (Modules::Led *)appMod[MOD_LED]; }
    Modules::Shell *shell() { return (Modules::Shell *)appMod[MOD_SHELL]; }
    Modules::Telnet *telnet() { return (Modules::Telnet *)appMod[MOD_TELNET]; }  
    Modules::Web *web() { return (Modules::Web *)appMod[MOD_WEB]; }
    
    PsuModule *psu() { return (PsuModule *)appMod[MOD_PSU]; }
    
    ConfigHelper *config() { return configHelper; }
    Config *params() { return configHelper->get(); }
    Display *lcd() { return (Display *)appMod[MOD_DISPLAY]; }
    
    PsuLogHelper *getPsuLog() { return psuLog; }
    LoopWatcher* watcher() {return loopLogger;}

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
        out_->print(millis() / ONE_SECOND_ms);
        out_->print(' ');
        out_->print(getLogLevel(buf, level));
        out_->print(' ');
        out_->println(msg);
        out_->flush();
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
    void restartNetworkDependedModules(NetworkMode mode, bool hasNetwork);
    void restart();
    void send_psu_data_to_clients();

   private:
    bool networkChanged;
    LoopWatcher *loopLogger;
    ConfigHelper *configHelper;
    PsuLogHelper *psuLog;
    WiFiEventHandler onDisconnected, onGotIp;
    unsigned long restartUpdated_;
    bool restartFlag_;
    time_t restartCountdown_;
    uint8_t boot_per;

   private:
    Module *appMod[APP_MODULES];

    ModuleDefine define[APP_MODULES] = {
        {str_btn, 0, false, false},
        {str_clock, 0, false, false},
        {str_web, 0, false, false},
        {str_display, 0, false, false},
        {str_led, 0, false, false},
        {str_netsvc, 0, false, false},
        {str_psu, 0, false, false},
        {str_shell, 0, false, false},
        {str_telnet, 0, false, false},
        {str_update, 0, false, false},
        {str_syslog, 0, false, false}};
};

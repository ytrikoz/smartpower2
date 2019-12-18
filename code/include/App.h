#pragma once

#include "Core/ModuleHost.h"

#include "Modules/Button.h"
#include "Modules/Clock.h"
#include "Modules/Led.h"
#include "Modules/Display.h"
#include "Modules/NetworkService.h"
#include "Modules/OTAUpdate.h"
#include "Modules/Psu.h"
#include "Modules/Syslog.h"
#include "Modules/Shell.h"
#include "Modules/Telnet.h"
#include "Modules/Web.h"

#include "CommonTypes.h"
#include "ConfigHelper.h"
#include "Looptiming.h"
#include "Plot.h"
#include "PsuLogHelper.h"

class App : public ModuleHost, PsuListener {
   public:
    App();

   public:
    void onPsuData(PsuData &item) override;

    private:
    void onConfigParamChange(ConfigItem param, const char* value);

   public:
    String getName(uint8_t index) const override;
    Module *getInstance(uint8_t index) const override;

   public:
    bool getByName(const char *str, ModuleEnum &module) const;
    Module *getInstanceByName(const String &name);
    void instanceMods();
    void initMods();
    void setupMods();

   public:
    void restart(time_t delay = 0);
    void handleRestart();
    void setConfig(ConfigHelper* config);
    void setPowerlog(PsuLogHelper *powerlog);

    void begin();
    void startSafe();
    void loop(LoopTimer *looptimer = nullptr);
    void loopSafe();

    size_t printDiag(Print *p);

    void printPlot(PlotData *data, Print *p);

    Modules::Display *display();
    Modules::Button *btn();
    Modules::Clock *clock();
    Modules::Led *led();
    Modules::Shell *shell();
    Modules::Telnet *telnet();
    Modules::Web *web();
    Modules::Psu *psu();

    Config *params() { return config_->get(); }
    
    bool setBootPowerState(BootPowerState state);

    bool setOutputVoltageAsDefault();

    uint8_t getTPW();

    void refresh_power_led();

    void refresh_wifi_led();

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
    void restart();

   private:
    bool networkChanged_;
    NetworkMode networkMode_;
    bool hasNetwork_;
    bool hasNetworkActivty_;
    
    ConfigHelper *config_;
    PsuLogHelper *powerlog_;
    LoopTimer *looptimer_;
    WiFiEventHandler onDisconnected, onGotIp;
    unsigned long restartUpdated_;
    bool restartFlag_;
    time_t restartCountdown_;
    uint8_t boot_per;

   private:
    ModuleDefinition define[APP_MODULES] = {
        {str_btn, 0, false, NETWORK_OFF},
        {str_clock, 0, false, NETWORK_OFF},
        {str_web, 0, false, NETWORK_STA},
        {str_display, 0, false, NETWORK_OFF},
        {str_led, 0, false, NETWORK_OFF},
        {str_netsvc, 0, false, NETWORK_STA},
        {str_psu, 0, false, NETWORK_OFF},
        {str_shell, 0, false, NETWORK_OFF},
        {str_telnet, 0, false, NETWORK_STA},
        {str_update, 0, false, NETWORK_STA},
        {str_syslog, 0, false, NETWORK_STA}};
};

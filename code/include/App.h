#pragma once

#include "Modules/Host.h"

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
#include "Powerlog.h"

class App : public Host, PsuDataListener {
   public:
    App();

   public:
    void onConfigChange(const ConfigItem param, const String& value);
    void onPsuData(PsuData &item) override;
    void onPsuStateChange(PsuState);
    void onPsuStatusChange(PsuStatus);
    void onWebStatusChange(bool connected);
    void onTelnetStatusChange(bool connected);

    void onNetworkStatusChange(bool has, NetworkMode mode);
    void setOutputVoltage(float value);

   public:
    String getName(uint8_t index) const override;
    Module *getInstance(uint8_t index) const override;

   public:
    bool getByName(const char *str, ModuleEnum &module) const;
    Module *getInstanceByName(const String &name);
    void instanceMods();
    void initMods();
    void setupMods();
    void systemRestart();
    void systemReset();

   public:
    void setConfig(ConfigHelper *config);
    void setPowerlog(PowerLog *powerlog);

    void begin();
    void startSafe();
    AppState loop(LoopTimer *looper = nullptr);
    void loopSafe();

    size_t printDiag(Print *p);

    void printPlot(PlotSummary *data, Print *p);

    Modules::Display *display();
    Modules::Button *btn();
    Modules::Clock *clock();
    Modules::Led *led();
    Modules::Console *console();
    Modules::Telnet *telnet();
    Modules::Web *web();
    Modules::Psu *psu();

    Config *params() { return config_->get(); }

    bool setBootPowerState(BootPowerState state);

    bool setOutputVoltageAsDefault();

    uint8_t getTPW();

    void refreshRed();

    void refreshBlue();

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
    AppState exitState_;
    bool exitFlag_;

    bool systemEvent_;
    bool networkEvent_;
    bool psuEvent_;

    bool hasNetwork_;
    NetworkMode networkMode_;
    bool webClients_;
    bool telnetClients_;

    ConfigHelper *config_;
    PowerLog *powerlog_;
    WiFiEventHandler onDisconnected, onGotIp;


    uint8_t boot_per;

   private:
    ModuleDef modules[MODULES_COUNT] = {
        {0, str_btn, NETWORK_OFF},
        {0, str_led, NETWORK_OFF},
        {0, str_clock, NETWORK_OFF},
        {0, str_psu, NETWORK_OFF},
        {0, str_display, NETWORK_OFF},
        {0, str_console, NETWORK_OFF},
        {0, str_netsvc, NETWORK_STA},
        {0, str_telnet, NETWORK_STA},
        {0, str_update, NETWORK_STA},
        {0, str_syslog, NETWORK_STA},
        {0, str_web, NETWORK_STA}};
};

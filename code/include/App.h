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
    void onConfigChange(const ConfigItem param, const String &value);
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
    void setConfig(ConfigHelper *);
    void setPowerlog(PowerLog *);
    void setWireless(Wireless *);

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
    Modules::Syslog *syslog();

    Config *params() { return config_->get(); }

    void refreshRed();

    void refreshBlue();

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

    Wireless *wireless_;
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
        {0, str_update, NETWORK_AP},
        {0, str_syslog, NETWORK_STA},
        {0, str_web, NETWORK_AP}};
};

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
#include "Modules/WebUI.h"

#include "CommonTypes.h"
#include "ConfigHelper.h"
#include "LoopTiming.h"
#include "Plot.h"
#include "PowerLog.h"

enum AppState {
    STATE_NORMAL,
    STATE_RESTART,
    STATE_FAILBACK
};

class App : public Host, PsuDataListener {
   public:
    App();

   public:
    void onConfigChange(const ConfigItem param, const String &value);
    void onPsuData(PsuData &item) override;
    void onPsuStateChange(PsuState);
    void onPsuError(Error);
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
    void initMods();
    void setupMods();
    void systemRestart();
    void systemReset();
    void setModules(ModuleDef *);

   public:
    void setConfig(ConfigHelper *);
    void setPowerlog(PowerLog *);
    void setWireless(Wireless *);

    void begin();
    AppState loop(LoopWatcher *looper = nullptr);
    size_t printDiag(Print *p);

    void printPlot(PlotSummary *data, Print *p);

    Modules::Display *display();
    Modules::Button *btn();
    Modules::Clock *clock();
    Modules::Led *led();
    Modules::Console *console();
    Modules::Telnet *telnet();
    Modules::WebUI *web();
    Modules::Psu *psu();
    Modules::Syslog *syslog();

    Config *params() { return config_->get(); }

    void updateRed(const bool activeState, const bool alertState, const bool errorState);
    void updateBlue(const bool activeState, const bool alertState, const bool errorState);
    void updateDisplay();

   private:
    void displayProgress(uint8_t progress, const char *message);
    void restart();
    void clearEvents();

   private:
    AppState state_;
    bool shutdown_;

    bool systemEvent_;
    bool networkEvent_;
    bool powerEvent_;

    bool hasNetwork_;
    bool hasWebClients_;
    bool hasTelnetClients_;

    NetworkMode networkMode_;

    Wireless *wireless_;
    ConfigHelper *config_;
    PowerLog *powerlog_;
    WiFiEventHandler onDisconnected, onGotIp;

    uint8_t boot_per;

   private:
    ModuleDef *modules_;
};

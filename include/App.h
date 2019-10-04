#pragma once

#include <Arduino.h>

#include "Modules/Button.h"
#include "Modules/Display.h"
#include "Modules/Leds.h"
#include "Modules/NetworkService.h"
#include "Modules/NtpClient.h"
#include "Modules/OTAUpdate.h"
#include "Modules/Psu.h"
#include "Modules/ShellMod.h"
#include "Modules/SystemClock.h"
#include "Modules/TelnetServer.h"
#include "Modules/WebService.h"

#include "ConfigHelper.h"
#include "LoopLogger.h"

#include "Strings.h"

class App {
  public:
    App();
    void init(Print *p);
    void start();
    bool start(const AppModuleEnum);
    void stop(const AppModuleEnum);
    bool getModule(String &str, AppModuleEnum &mod);
    bool getModule(const char *str, AppModuleEnum &mod);
    AppModule *getInstance(const AppModuleEnum);
    void loop();
    void restart(uint8_t delay_s);
    void printConfig(Print *p);
    void printDiag(const AppModuleEnum module, Print *p);
    void printLoopCapture(Print *p);
    LoopLogger *getLoopLogger();
    size_t printDiag(Print *p);
    size_t printDiag(Print *p, const AppModuleEnum);
    void printCapture(Print *);
    void resetConfig();
    void loadConfig();
    bool saveConfig();
    Config *getConfig();
    String getNetworkConfig();
    Display *getDisplay();
    SystemClock *getClock();
    Psu *getPsu();
    PsuState *getPsuState();
    bool setBootPowerState(BootPowerState state);
    bool setOutputVoltageAsDefault();
    uint8_t getTPW();
    void refresh_power_led();
    void refresh_wifi_led();

  private:
    bool isNetworkDepended(AppModuleEnum module);
    void refresh_network_modules(bool hasNetwork);
    void handle_restart();
    void boot_progress(uint8_t per, const char *payload = NULL);
    void send_psu_data_to_clients();

  private:
    WiFiEventHandler onDisconnected, onGotIp;
    unsigned long displayUpdated;
    uint8_t reboot;
    uint8_t boot_per;
    LoopLogger *loopLogger;
    ConfigHelper *env;
    ShellMod *shell;
    OTAUpdate *ota;
    Display *display;
    SystemClock *rtc;
    NtpClient *ntp;
    Psu *psu;
    Button *btn;
    Led::Leds *leds;
    NetworkService *discovery;
    AppModule *appMod[APP_MODULES];
    Print *out;
    Print *dbg;
    Print *err;
};

extern App app;
#pragma once

#include <Arduino.h>

#include "ConfigHelper.h"
#include "LoopLogger.h"
#include "Plot.h"
#include "PsuLogger.h"
#include "Strings.h"

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
    void restart(uint8_t time);
    void printConfig(Print *p);
    size_t printDiag(Print *p);
    size_t printDiag(Print *p, const AppModuleEnum module);
    void printLoopCapture(Print *p);
    void printPlot(PlotData *data, Print *p);

    void printCapture(Print *);
    void resetConfig();
    void loadConfig();
    bool saveConfig();
    Config *getConfig();
    LoopLogger *getLoopLogger();

    String getNetworkConfig();
    SystemClock *getClock();
    Psu *getPsu();
    ShellMod *getShell();
    WebService *getHttp();
    bool setBootPowerState(BootPowerState state);
    bool setOutputVoltageAsDefault();
    uint8_t getTPW();
    void refresh_power_led();
    void refresh_wifi_led();
    Display *getDisplay();

  private:
    uint8_t get_telnet_clients_count();
    uint8_t get_http_clients_count();
    bool isNetworkDepended(AppModuleEnum module);
    void refresh_network_modules(bool hasNetwork);
    void handle_restart();
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
    WebService *http;
    TelnetServer *telnet;
    SystemClock *rtc;
    NtpClient *ntp;
    PsuLogger *logger;
    Psu *psu;
    Button *btn;
    Led::Leds *leds;
    NetworkService *discovery;
    AppModule *appMod[APP_MODULES];
    Print *out, *dbg, *err;
};

extern App app;
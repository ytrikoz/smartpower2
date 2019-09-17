#pragma once

#include <IPAddress.h>
#include <SimpleCLI.h>
#include <SimpleTimer.h>
#include <mcurses.h>

#include "Backlight.h"
#include "CommonTypes.h"
#include "ConfigHelper.h"
#include "Display.h"
#include "Led.h"
#include "LoopWatchDog.h"
#include "NetworkService.h"
#include "NtpClient.h"
#include "OTAUpdate.h"
#include "Psu.h"
#include "PsuLogger.h"
#include "Plot.h"
#include "Shell.h"
#include "SystemClock.h"
#include "TelnetServer.h"
#include "WebService.h"

extern AppModule* appModule[];
extern ConfigHelper *config;
extern Display *display;
extern Led::Led *wifi_led, *power_led;
extern Profiler::LoopWatchDog watchDog;
extern SimpleCLI *cli;
extern SimpleTimer timer;
extern NetworkService *discovery;
extern NtpClient *ntp;
extern OTAUpdate *ota;
extern Psu *psu;
extern PsuLogger *psuLog;
extern Shell *telnetShell;
extern Shell *consoleShell;
extern TelnetServer *telnet;
extern WebClient clients[MAX_WEB_CLIENTS];
extern WebService *http;

void start_clock();
void init_psu();

void start_services();
void start_ntp();
void start_http();
void start_telnet();
void start_ota_update();
void start_discovery();

void start_console_shell();
void start_telnet_shell(Stream *s);

void refresh_wifi_led();

void update_display();

uint8_t get_telnet_clients_count();

void onTimeChangeEvent(const char *str);

// main
extern void sendPageState(uint8_t page);
extern void onHttpClientConnect(uint8_t num);
extern void onHttpClientDisconnect(uint8_t num);
extern void onHttpClientData(uint8_t num, String data);

extern void cancel_system_restart();
extern void setup_restart_timer(uint8_t delay_s = 3);

extern uint8_t get_http_clients_count();

// plot
size_t fill_data(PlotData *data, float *vals, size_t size);

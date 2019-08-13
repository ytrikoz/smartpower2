#pragma once

#include <IPAddress.h>
#include <SimpleCLI.h>
#include <SimpleTimer.h>
#include <mcurses.h>

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
#include "Shell.h"
#include "SystemClock.h"
#include "TelnetServer.h"
#include "WebService.h"

extern Led *wifi_led, *power_led;

extern SimpleCLI *cli;
extern SimpleTimer timer;

extern NetworkService *discovery;
extern ConfigHelper *config;
extern Display *display;
extern Psu *psu;
extern PsuLogger *psuLog;
extern NtpClient *ntp;
extern OTAUpdate *ota;
extern TelnetServer *telnet;
extern WebService *http;
extern Shell *telnetShell;
extern Shell *consoleShell;

void start_clock();
void start_psu();
void start_ntp();
void start_http();
void start_telnet();
void start_ota_update();
void start_discovery();
bool start_telnet_shell(Stream *s);
void start_services();
void refresh_wifi_led();
void update_display_every_1_sec();
void update_display_every_5_sec();
uint8_t get_telnet_clients_count();
void start_console_shell();
void onSystemTimeChanged(const char *str);
// main
extern void onHttpClientConnect(uint8_t num);
extern void onHttpClientDisconnect(uint8_t num);
extern void onHttpClientData(uint8_t num, String data);

extern void cancel_system_restart();
extern void setup_restart_timer(uint8_t delay_s = 3);

extern uint8_t get_http_clients_count();
extern LoopWatchDog loopWD;

// plot
size_t fill_data(PlotData *data, float *vals, size_t size);

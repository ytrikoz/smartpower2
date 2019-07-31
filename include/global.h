#pragma once

#include "types.h"

#include "mcurses.h"
#include <IPAddress.h>
#include <SimpleCLI.h>
#include <SimpleTimer.h>
#include "SystemClock.h"

#include "Led.h"
#include "Shell.h"
#include "ConfigHelper.h"
#include "Display.h"
#include "PSU.h"
#include "NTPClient.h"
#include "NetworkService.h"
#include "OTAUpdate.h"
#include "TelnetServer.h"
#include "WebService.h"

extern Led *wifi_led, *power_led;

extern SimpleCLI *cli;
extern SimpleTimer timer;

extern NetworkService *discovery;
extern ConfigHelper *config;
extern Display *display;
extern PSU *psu;
extern NTPClient *ntp;
extern OTAUpdate *ota;
extern TelnetServer *telnet;
extern WebService *http;
extern Shell *telnetShell;
extern Shell *consoleShell;

String getLoopStat();
void start_clock();
void start_psu();
void start_ntp();
void start_http();
void start_telnet();
void start_ota_update();
void start_discovery();
bool start_telnet_shell(Stream *s);
void start_services();
void refresh_wifi_status_led();
uint8_t get_telnet_clients_count();
void start_console_shell();
void onSystemTimeChanged(const char* str);
// main
extern void onHttpClientConnect(uint8_t num);
extern void onHttpClientDisconnect(uint8_t num);
extern void onHttpClientData(uint8_t num, String data);
extern void cancel_system_restart();
extern void setup_restart_timer(uint8_t count);
extern uint8_t get_http_clients_count();
extern ulong get_lps();
extern ulong get_longest_loop();
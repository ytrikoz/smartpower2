#pragma once

#include "types.h"

#include <IPAddress.h>
#include <SimpleCLI.h>
#include <SimpleTimer.h>
#include "SystemClock.h"

#include "cli.h"
#include "ConfigHelper.h"
#include "Display.h"
#include "Multimeter.h"
#include "NTPClient.h"
#include "NetworkService.h"
#include "OTAUpdate.h"
#include "Shell.h"
#include "TelnetServer.h"
#include "WebService.h"

extern SimpleCLI *cli;
extern SimpleTimer timer;

extern NetworkService *discovery;
extern ConfigHelper *config;
extern Display *display;
extern Multimeter *meter;
extern NTPClient *ntp;
extern OTAUpdate *ota;
extern Shell *t_sh;
extern Shell *s_sh;

extern TelnetServer *telnet;
extern WebService *http;

extern void onHttpClientConnect(uint8_t num);
extern void onHttpClientDisconnect(uint8_t num);
extern void onHttpClientData(uint8_t num, String data);

extern String getLoopStat();

extern void start_services();
extern void start_serial_shell(Stream *);
extern bool start_telnet_cli(Stream *);
extern void printWelcomeTo(Print *p);

// main
extern PowerState get_power_state();
extern void set_power_state(PowerState state);
extern void cancel_system_restart();
extern bool start_telnet_shell(Stream *s);
extern void setup_restart_timer(int s);
uint8_t get_http_clients_count();
uint8_t get_telnet_clients_count();
extern ulong get_lps();
extern ulong get_longest_loop();
extern String get_system_time();

void start_ntp();
void start_http();
void start_ota_update();
void start_clock();
void start_telnet();
void start_discovery();

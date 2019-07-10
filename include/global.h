#pragma once
#include <SimpleTimer.h>
#include <SimpleCLI.h>

#include "consts.h"
#include "cli.h"

#include "ConfigHelper.h"
#include "Multimeter.h"
#include "Shell.h"
#include "Multimeter.h"
#include "Display.h"
#include "WebService.h"
#include "TelnetServer.h"
#include "NTPClient.h"
#include "NetworkService.h"
#include "SystemClock.h"
#include "OTAUpdate.h"

extern TelnetServer *telnet;
extern NetworkServices *network_discovery;
extern SimpleTimer timer;
extern Multimeter *meter;
extern SimpleCLI *cli;
extern NTPClient *ntp;
extern Shell *serial_shell;
extern Shell *telnet_shell;

extern void setup_telnet();
extern void setup_network_discovery();
extern void setup_clock();

extern bool init_telnet_shell(Stream*);
extern void init_serial_shell(Stream*);
extern String get_system_time();

// main
extern PowerState get_power_state();
extern void set_power_state(PowerState state);

extern void cancel_system_restart();
extern void setup_restart_timer(int s);

uint8_t get_http_clients_count();
uint8_t get_telnet_clients_count();

extern ulong get_lps();
extern ulong get_longest_loop();
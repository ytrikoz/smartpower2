#pragma once

#include <IPAddress.h>

#include <SimpleCLI.h>
#include <SimpleTimer.h>
#include <mcurses.h>

#include "App.h"
#include "CommonTypes.h"
#include "Plot.h"

extern App app;

extern SimpleCLI *cli;
extern SimpleTimer timer;
extern NetworkService *discovery;
extern OTAUpdate *ota;
extern Psu *psu;
extern PsuLogger *psuLogger;
extern TelnetServer *telnet;
extern WebClient clients[MAX_WEB_CLIENTS];
extern WebService *http;

void update_display();

uint8_t get_telnet_clients_count();

void load_screen_message(const char *type, const char *msg);
void load_screen_psu_pvi();

// main
extern void sendPageState(uint8_t page);
extern void onHttpClientConnect(uint8_t num);
extern void onHttpClientDisconnect(uint8_t num);
extern void onHttpClientData(uint8_t num, String data);
extern void sendToClients(String payload, uint8_t page);
extern uint8_t get_http_clients_count();

// plot
size_t fill_data(PlotData *data, float *vals, size_t size);

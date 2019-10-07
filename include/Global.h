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
extern WebClient clients[MAX_WEB_CLIENTS];

// main
extern void sendPageState(uint8_t page);
extern void onHttpClientConnect(uint8_t num);
extern void onHttpClientDisconnect(uint8_t num);
extern void onHttpClientData(uint8_t num, String data);
extern void sendToClients(String payload, uint8_t page);

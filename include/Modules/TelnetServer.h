#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include "AppModule.h"
#include "BuildConfig.h"
#include "Consts.h"
#include "StrUtils.h"
#include "Wireless.h"

typedef std::function<bool(Stream *)> TelnetConnectEventHandler;
typedef std::function<void()> TelnetDisconnectEventHandler;

class TelnetServer : public AppModule {
   public:
    TelnetServer();
    bool begin();
    void stop();
    void loop();
    void write(const char *);
    bool hasClientConnected();
    void setOnClientConnect(TelnetConnectEventHandler handler);
    void setOnCLientDisconnect(TelnetDisconnectEventHandler handler);

   private:
    void init();
    void onConnect();
    void onDisconnect();
    uint16_t port;
    bool active;
    bool initialized;
    bool connected;
    WiFiClient client;
    WiFiServer *server;
    TelnetConnectEventHandler onConnectEvent;
    TelnetDisconnectEventHandler onDisconnectEvent;
    WiFiEventHandler staDisconnected, staGotIP;
    Print *output = &USE_SERIAL;
};
#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include "consts.h"
#include "str_utils.h"
#include "wireless.h"

typedef std::function<bool(Stream *)> TelnetConnectEventHandler;
typedef std::function<void()> TelnetDisconnectEventHandler;

class TelnetServer {
   public:
    TelnetServer(uint16_t port);
    void setOutput(Print *p);

    void begin();
    void stop();
    void write(const char *);
    void loop();

    bool hasClientConnected();
    void setOnClientConnect(TelnetConnectEventHandler handler);
    void setOnCLientDisconnect(TelnetDisconnectEventHandler handler);

   private:
    void init();
    
    void onConnect();
    void onDisconnect();

    bool active;
    bool connected;
    bool initialized;    
    uint16_t port;

    WiFiClient client;
    WiFiServer *server;

    TelnetConnectEventHandler onConnectEvent;
    TelnetDisconnectEventHandler onDisconnectEvent;

    WiFiEventHandler onDisconnected, onGotIp;

    Print *output = &USE_SERIAL;
};
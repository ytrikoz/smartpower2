#pragma once

#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

#include "AppModule.h"
#include "BuildConfig.h"
#include "Consts.h"
#include "StrUtils.h"
#include "Wireless.h"

enum TelnetEventType { CLIENT_CONNECTED, CLIENT_DISCONNECTED, CLIENT_DATA };

typedef std::function<bool(TelnetEventType, Stream *)> TelnetEventHandler;

class TelnetServer : public AppModule {
  public:
    TelnetServer();
    void setConfig(Config *cfg);
    bool begin();
    void end();
    void loop();

  public:
    void setEventHandler(TelnetEventHandler);
    void write(const char *);
    bool hasClient();

  private:
    void onConnect();
    void onDisconnect();
    void onData();
    uint16_t port;
    bool active;
    bool connected;
    WiFiClient client;
    WiFiServer *server = nullptr;
    TelnetEventHandler eventHandler;
};
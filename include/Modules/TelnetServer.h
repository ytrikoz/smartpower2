#pragma once

#include <ESP8266WiFi.h>

#include "AppModule.h"
#include "BuildConfig.h"
#include "Consts.h"
#include "StrUtils.h"
#include "Wireless.h"

enum TelnetEventType { CLIENT_CONNECTED,
                       CLIENT_DISCONNECTED,
                       CLIENT_DATA };

typedef std::function<bool(TelnetEventType, Stream *)> TelnetEventHandler;

class TelnetServer : public AppModule {
   public:
    void setEventHandler(TelnetEventHandler);

    void write(const char *);

    bool hasClient();

    bool isCompatible(Wireless::NetworkMode value) {
        return value != Wireless::NETWORK_OFF;
    }

    bool isNetworkDepended() { return true; }

   public:
    TelnetServer() : AppModule(MOD_TELNET) {
        lastConnected_ = false;
        port_ = TELNET_PORT;
    }

   protected:
    bool onInit();
    bool onStart();
    void onStop();
    void onLoop();
    size_t printDiag(Print *p);

   private:
    void onConnect();
    void onDisconnect();
    void onData();

   private:
    uint16_t port_;
    bool lastConnected_;
    WiFiClient client_;
    WiFiServer *server_ = 0;
    TelnetEventHandler eventHandler_;
};
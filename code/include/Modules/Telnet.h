#pragma once

#include <ESP8266WiFi.h>

#include "Module.h"
#include "BuildConfig.h"
#include "Consts.h"
#include "StrUtils.h"
#include "Terminal.h"
#include "Wireless.h"

enum TelnetEventType { 
                       CLIENT_CONNECTED,
                       CLIENT_DISCONNECTED
                    };

typedef std::function<bool(TelnetEventType, WiFiClient *)> TelnetEventHandler;

namespace Modules {

class Telnet : public Module {
    public:
    Telnet() : Module(), lastConnected_(false), port_(TELNET_PORT){};

   public:
    void setEventHandler(TelnetEventHandler);
    Terminal* getTerminal();    
    void sendData(const String&);
    bool hasClient();

    bool isCompatible(NetworkMode value) {
        return value != NetworkMode::NETWORK_OFF;
    }

    bool isNetworkDepended() { return true; }

   protected:
    bool onInit() override;
    bool onStart() override;
    void onStop() override;
    void onLoop() override;
    size_t onDiag(Print *p) override;

   private:
    void onConnect();
    void onDisconnect();
    void onData();
    void onOpen();
    void onClose();    
   private:
    TelnetEventHandler eventHandler_;
    TerminalEventHandler terminalHandler_;
    bool lastConnected_;
    uint16_t port_;
    WiFiClient client_;
    Terminal* terminal_;
    WiFiServer *server_;
};

}
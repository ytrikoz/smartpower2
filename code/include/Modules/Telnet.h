#pragma once

#include <ESP8266WiFi.h>

#include "Module.h"
#include "BuildConfig.h"
#include "Consts.h"
#include "StrUtils.h"
#include "Terminal.h"
#include "Wireless.h"

#include "CommandShell.h"

enum TelnetEventType { 
                       CLIENT_CONNECTED,
                       CLIENT_DISCONNECTED
                    };

typedef std::function<bool(TelnetEventType, WiFiClient *)> TelnetEventHandler;

namespace Modules {

class Telnet : public Module {
    public:
    Telnet() : Module(), lastConnected_(false), port_(TELNET_PORT){};
    void setShell(CommandShell* shell);
   public:
    void setEventHandler(TelnetEventHandler);
    void sendData(const String&);
    bool hasClient();

    bool isCompatible(NetworkMode value) {
        return value != NetworkMode::NETWORK_OFF;
    }
    bool isNetworkDepended() { return true; }
    
    void onDiag(const JsonObject& doc) override;
   protected:
    bool onInit() override;
    bool onStart() override;
    void onStop() override;
    void onLoop() override;

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
    Terminal term_;
    CommandShell* shell_;
    WiFiServer *server_;
};

}
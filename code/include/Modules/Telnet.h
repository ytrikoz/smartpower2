#pragma once

#include <ESP8266WiFi.h>

#include "Core/Module.h"
#include "BuildConfig.h"
#include "Consts.h"
#include "Utils/StrUtils.h"
#include "Wireless.h"
#include "Cli/CommandShell.h"

enum TelnetEventType {
    CLIENT_CONNECTED,
    CLIENT_DISCONNECTED
};

typedef std::function<void(TelnetEventType, WiFiClient*)> TelnetEventHandler;

namespace Modules {

class Telnet : public Module {
   public:
    Telnet(uint16_t port): lastConnected_(false), port_(port){};
   public:
    void setEventHandler(TelnetEventHandler);
    void sendData(const String&);
    bool hasClient();
    void onDiag(const JsonObject& doc) override;

   protected:
    bool onInit() override;
    void onDeinit() override;
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
    bool lastConnected_;
    uint16_t port_;
    WiFiClient client_;
    WiFiServer* server_;
    Cli::Terminal* term_;
    Cli::CommandShell* shell_;

};

}  // namespace Modules
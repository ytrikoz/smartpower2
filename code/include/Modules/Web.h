#pragma once

#include <Arduino.h>

#include "Modules/Module.h"
#include "WebServer/WebServer.h"
#include "WebServer/WebServerAsync.h"

namespace Modules {

class Web : public Module {
   public:
    void sendToClients(const String&, const uint8_t);
    void sendToClients(const String&, const uint8_t, const uint8_t);
    void sendPageState(const uint8_t page);
    void sendPageState(const uint8_t n, const uint8_t page);
    uint8_t getClients(); 

   public:
    Web() : Module() {}

    bool isCompatible(NetworkMode value) override { return value != NetworkMode::NETWORK_OFF; }

    bool isNetworkDepended() { return true; }

   protected:
    bool onInit() override;
    bool onStart() override;
    void onStop() override;
    void onLoop() override;

   private:
    void onWebClientConnect(uint8_t);
    void onWebClientDisconnect(uint8_t);
    void onWebClientData(const uint8_t n, const String& data);

   private:
    WebClient clients_[WEB_SERVER_CLIENT_MAX];
    WebServer *web_;
};

}
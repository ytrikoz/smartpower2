#pragma once

#include <Arduino.h>

#include "AppModule.h"
#include "WebServer/WebServer.h"
#include "WebServer/WebServerAsync.h"

class WebMod : public AppModule {
   public:
    void sendToClients(String&, const uint8_t);
    void sendToClients(String&, const uint8_t, const uint8_t);
    void sendPageState(const uint8_t page);
    void sendPageState(const uint8_t n, const uint8_t page);
    uint8_t getClients(); 

   public:
    WebMod() : AppModule(MOD_WEB) {}

    bool isCompatible(Wireless::NetworkMode value) override { return value != Wireless::NETWORK_OFF; }

    bool isNetworkDepended() { return true; }

   protected:
    bool onInit() override;
    bool onStart() override;
    void onStop() override;
    void onLoop() override;

   private:
    void onHttpClientConnect(uint8_t);
    void onHttpClientDisconnect(uint8_t);
    void onHttpClientData(uint8_t n, String data);

   private:
    WebClient clients_[WEB_SERVER_CLIENT_MAX];
    WebServer *web_;
};

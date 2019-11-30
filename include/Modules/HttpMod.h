#pragma once

#include <Arduino.h>

#include "AppModule.h"
#include "WebService.h"

class HttpMod : public AppModule {
   public:

    void sendToClients(String, uint8_t);
    void sendToClients(String, uint8_t, uint8_t);
    void sendPageState(uint8_t page);
    void sendPageState(uint8_t n, uint8_t page);
    uint8_t getClients();
    
   public:
    HttpMod() : AppModule(MOD_HTTP) {}

    bool isCompatible(Wireless::NetworkMode value) override {
        return value != Wireless::NETWORK_OFF;
    }

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
    WebClient clients_[MAX_WEB_CLIENTS];
    WebService *http_;
};

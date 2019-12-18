#pragma once

#include "Core/Module.h"

#include "WebServer/WebServer.h"
#include "WebServer/WebServerAsync.h"

namespace Modules {

class Web : public Module {
   public:
    Web();

   protected:
    bool onInit() override;
    bool onStart() override;
    void onStop() override;
    void onLoop() override;

   public:
    void sendPageState(uint8_t);
    void sendPageState(uint8_t, uint8_t);
    void sendToClients(const String&, uint8_t);
    void sendToClients(const String&, uint8_t, uint8_t);
    uint8_t getClients();

   private:
    void onConnection(uint8_t);
    void onDisconnection(uint8_t);
    void onData(uint8_t n, const String& data);

   private:
    WebClient session_[WEB_SERVER_CLIENT_MAX];
    WebServer* web_;
};

}  // namespace Modules
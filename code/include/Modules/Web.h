#pragma once

#include "Core/Module.h"

#include "WebServer/WebServer.h"
#include "WebServer/WebServerAsync.h"

struct WebClient {
    bool connected;
    uint32_t num;
    WebPageEnum page;
};

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
    void sendPage(const WebPageEnum page, const uint32_t num = 0);
    void sendAll(const String& data, WebPageEnum page, uint32_t except_num = 0);
    size_t getClients();

   private:
    void updateStaticJson();
    void fillInfo(JsonObject& obj);
    void fillOptions(JsonObject& obj);
    void fillMain(JsonObject& obj);
    String getPageData(const WebPageEnum page);
    void onConnection(const uint32_t num, const bool conntected);
    void onDisconnection(const uint32_t num);
    void onData(const uint32_t num, const String& data);
    bool getFreeSlot(WebClient**c);
    bool getClientByNum(uint32_t num, WebClient **c);
   private:
    size_t cnt_;
    WebClient client_[WEB_SERVER_CLIENT_MAX];
    WebServer* web_;
};

}  // namespace Modules
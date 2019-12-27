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

class Web : public Module, public WebServerHandler {
   public:
    void onConnection(const uint32_t num, const bool connected) override;
    void onData(const uint32_t num, const String& data) override;
    bool getResponse(const String& uri, String& body) override;
    bool uriExist(const String& uri, String& lastModified) override;

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
    String getPageData(const WebPageEnum page);

    void fillOptions(JsonObject& obj);
    void fillMain(JsonObject& obj);
    void fillNetwork(JsonObject& obj);
    void fillVersion(JsonObject& obj);
    void fillSystem(JsonObject& obj);

    bool getFreeSlot(WebClient** c);
    bool getClientByNum(uint32_t num, WebClient** c);

   private:
    size_t cnt_;
    WebClient client_[WEB_SERVER_CLIENT_MAX];
    WebServer* web_;
    char last_modified_[50];
};

}  // namespace Modules
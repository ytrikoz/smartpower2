#pragma once

#include "Core/Module.h"
#include "WebServer/WebServer.h"
#include "WebServer/WebServerAsync.h"
#include "WebServer/WebHandler.h"
struct WebClient {
    bool connected;
    uint32_t num;
    WebPageEnum page;
};

namespace Modules {

class WebUI : public Module, public WebServerHandler {
   public:
    void onConnection(const uint32_t num, const bool connected) override;
    void onData(const uint32_t num, const String& data) override;
    bool getResponse(const String& uri, String& body) override;
    bool exists(const String& uri, String& lastModified) override;

   public:
    WebUI();

   protected:
    bool onInit() override;
    bool onStart() override;
    void onStop() override;
    void onLoop() override;

   public:
    void send(const String& data, const WebPageEnum page, const uint32_t num);
    void updatePage(const WebPageEnum page);
    void broadcast(const String& data, const WebPageEnum page, const uint32_t num = 0);
    size_t getClients();

   private:
    String getPageData(const WebPageEnum page);

    void fillOptions(JsonObject& obj);
    void fillMain(JsonObject& obj);
    void fillNetwork(JsonObject& obj);
    void fillVersion(JsonObject& obj);
    void fillSystem(JsonObject& obj);

    bool getSlot(WebClient** c);
    bool getClient(uint32_t num, WebClient** c);
    void setClientPage(const uint32_t num, const WebPageEnum page);

   private:
    WebClient client_[WEB_SERVER_CLIENT_MAX];
    size_t client_cnt_;
    char last_modified_[50];
    WebServer* web_;
};

}  // namespace Modules
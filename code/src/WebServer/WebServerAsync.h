#pragma once

#include <Arduino.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#include "WebServer.h"

class WebServerAsync : public WebServer {
  public:
    WebServerAsync(uint16_t port);
    
    bool start() override;
    void stop() override;
    void loop() override;
    
    void sendData(uint8_t num, const char* payload) override;
    void sendData(uint8_t, const String &) override;
    void sendEvent(const String& content, const String& name) override;
    void setOnConnection(WebServerConnectionEventHandler) override;
    void setOnDisconnection(WebServerConnectionEventHandler) override;
    void setOnReceiveData(WebServerDataEventHandler) override;

  private:
    void onNotFound(AsyncWebServerRequest *r);
    void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

private:
    WebServerDataEventHandler dataHandler;
    WebServerConnectionEventHandler connectionHandler;
    WebServerConnectionEventHandler disconnectionHandler;

    AsyncWebServer *web_;
    AsyncWebSocket *ws_;

    char last_modified[50];
};
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
    
    void sendData(uint32_t, const String &) override;
    void sendEvent(const String&, const String&) override;
    void setOnConnection(WebClientConnectionEventHandler) override;
    void setOnData(WebClientDataEventHandler) override;
  private:
    void onNotFound(AsyncWebServerRequest *r);
    void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

private:
    WebClientDataEventHandler dataHandler;
    WebClientConnectionEventHandler connectionHandler;

    AsyncWebServer *web_;
    AsyncWebSocket *ws_;

    char last_modified[50];
};
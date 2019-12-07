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
    void sendData(const uint8_t, const String &) override;
    void setOnConnection(WebConnectionEventHandler) override;
    void setOnDisconnection(WebConnectionEventHandler) override;
    void setOnReceiveData(WebDataEventHendler) override;

  private:
    void onNotFound(AsyncWebServerRequest *r);
    void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

    WebDataEventHendler onReceiveDataEvent;
    WebConnectionEventHandler onConnectionEvent, onDisconnectionEvent;
    AsyncWebServer *web_;
    AsyncWebSocket *ws_;
    char last_modified[50];
};
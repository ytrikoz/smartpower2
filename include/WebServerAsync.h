#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>

#include "BuildConfig.h"
#include "Consts.h"
#include "StrUtils.h"
#include "SysInfo.h"

typedef std::function<void(uint8_t)> SocketConnectionEventHandler;
typedef std::function<void(uint8_t, String)> SocketDataEventHandler;

class WebServerAsync {
  public:
    WebServerAsync();
    bool start();
    void stop();
    void loop();
    size_t printDiag(Print *);

  public:
    void setOnClientConnection(SocketConnectionEventHandler);
    void setOnClientDisconnected(SocketConnectionEventHandler);
    void setOnClientData(SocketDataEventHandler);
    void sendTxt(uint8_t num, String &str);

  private:
    bool captivePortal();
    bool sendFile(String uri);
    bool sendFileContent(String path);
    String getFilePath(String uri);
    void handleUpload();
    void handleRoot();
    void handleUri();
    void handleFileList();
    void handleNoContent();
    void onNotFound(AsyncWebServerRequest *r);
    void onWSEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len);

    SocketDataEventHandler onDataEvent;
    SocketConnectionEventHandler onConnectEvent, onDisconnectEvent;
    AsyncWebServer *web_;
    AsyncWebSocket *ws_;
    AsyncEventSource *events_;
    File fsUploadFile;
};
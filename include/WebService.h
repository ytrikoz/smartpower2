#pragma once

#include <Arduino.h>
#include <ESP8266SSDP.h>
#include <ESP8266WebServer.h>
#include <FS.h>

#include <WebSocketsServer.h>

#include "BuildConfig.h"
#include "Consts.h"
#include "StrUtils.h"
#include "SysInfo.h"

typedef std::function<void(uint8_t)> SocketConnectionEventHandler;
typedef std::function<void(uint8_t, String)> SocketDataEventHandler;

class WebService {
  public:
    WebService(uint16_t http, uint16_t websocket);
    bool start();
    void stop();
    void loop();
    size_t printDiag(Print *);

  public:
    void setRoot(const char *path);
    void setOnClientConnection(SocketConnectionEventHandler h);
    void setOnClientDisconnected(SocketConnectionEventHandler h);
    void setOnClientData(SocketDataEventHandler h);
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
    void handleNotFound(String &uri);
    const char *getContentType(String filename);
    void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                              size_t lenght);

    SocketDataEventHandler onDataEvent;
    SocketConnectionEventHandler onConnectEvent, onDisconnectEvent;
    int16_t port_, wsport_;
    char root_[16];

    ESP8266WebServer *server;
    WebSocketsServer *socket;
    SSDPClass *ssdp;
    File fsUploadFile;
};
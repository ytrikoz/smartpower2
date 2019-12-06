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

class WebServer {
  public:
    WebServer(uint16_t http, uint16_t websocket);
    bool start();
    void stop();
    void loop();
    size_t printDiag(Print *);

  public:
    void setOnClientConnection(SocketConnectionEventHandler);
    void setOnClientDisconnected(SocketConnectionEventHandler);
    void setOnClientData(SocketDataEventHandler);
    void sendTxt(const uint8_t, String &);

  private:
    bool captivePortal();
    bool sendFile(String);
    bool sendFileContent(String);
    String getFilePath(String);
    String getContentType(String);
    void handleUpload();
    void handleRoot();
    void handleUri();
    void handleFileList();
    void handleNoContent();
    void handleNotFound(String &);
    
    void handleWebSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                              size_t lenght);

    SocketDataEventHandler onDataEvent;
    SocketConnectionEventHandler onConnectEvent, onDisconnectEvent;
    ESP8266WebServer *server;
    WebSocketsServer *socket;
    SSDPClass *ssdp;
    File fsUploadFile;
};
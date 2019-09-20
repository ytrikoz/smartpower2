#pragma once

#include <Arduino.h>
#include <ESP8266SSDP.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>

#include "AppModule.h"
#include "BuildConfig.h"
#include "Consts.h"
#include "StrUtils.h"
#include "SysInfo.h"

typedef std::function<void(uint8_t)> SocketConnectionEventHandler;
typedef std::function<void(uint8_t, String)> SocketDataEventHandler;

class WebService : public AppModule {
   public:
    WebService();
    bool begin();
    void end();
    void loop();
    void printDiag(Print*);
   public:
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
    void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                        size_t lenght);

    SocketDataEventHandler onDataEvent;
    SocketConnectionEventHandler onConnectEvent, onDisconnectEvent;

    bool active = false;
    int16_t port_http, port_websocket;
    char *web_root;

    ESP8266WebServer *server;
    WebSocketsServer *websocket;
    SSDPClass *ssdp;
    File fsUploadFile;
};
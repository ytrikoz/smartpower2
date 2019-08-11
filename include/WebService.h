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
    WebService(uint16_t http_port, uint16_t ws_port, const char *root);
    void setOutput(Print *output);
    void setOnClientConnection(SocketConnectionEventHandler h);
    void setOnClientDisconnected(SocketConnectionEventHandler h);
    void setOnClientData(SocketDataEventHandler h);
    void begin();
    void loop();
    void sendTxt(uint8_t, const char *);
    bool captivePortal();

   private:
    bool active = false;
    int16_t http_port, socket_port;
    char root[sizeof(WEB_ROOT)];

    bool sendFile(String uri);
    bool sendFileContent(String path);
    String getFilePath(String uri);
    const char *getContentType(String filename);
    void noContent();
    void handleRoot();
    void handleUri();
    void handleFileList();
    void handleNotFound(String& uri);
    void fileUpload();

    void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                        size_t lenght);

    SocketDataEventHandler onDataEvent;
    SocketConnectionEventHandler onConnectEvent, onDisconnectEvent;

    ESP8266WebServer *server;
    WebSocketsServer *websocket;
    SSDPClass *ssdp;
    Print *output;
};
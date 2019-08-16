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
    WebService();
    void setOutput(Print *output);
    void setOnClientConnection(SocketConnectionEventHandler h);
    void setOnClientDisconnected(SocketConnectionEventHandler h);
    void setOnClientData(SocketDataEventHandler h);
    void begin();
    void end();
    void loop();
    void sendTxt(uint8_t, const char *);

   private:
    bool captivePortal();

    bool sendFile(String uri);
    bool sendFileContent(String path);
    String getFilePath(String uri);
    const char *getContentType(String filename);
    void noContent();
    void handleRoot();
    void handleUri();
    void handleFileList();
    void handleNotFound(String &uri);
    void fileUpload();

    void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                        size_t lenght);

    SocketDataEventHandler onDataEvent;
    SocketConnectionEventHandler onConnectEvent, onDisconnectEvent;


    bool active = false;
    int16_t port_http, port_websocket;
    IPAddress ip;
    char web_root[sizeof(HTTP_WEB_ROOT)];
    
    ESP8266WebServer *server;
    WebSocketsServer *websocket;
    SSDPClass *ssdp;
    Print *output;
};
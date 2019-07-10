#pragma once
#include <Arduino.h>
#include <ESP8266SSDP.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include <WebSocketsServer.h>

#include "consts.h"
#include "ip_utils.h"
#include "str_utils.h"
#include "sysinfo.h"

typedef std::function<void(uint8_t, bool)> WebSocketConnectionCallback;
typedef std::function<void(uint8_t, String)> WebSocketDataCallback;

class WebService {
   public:
    WebService(IPAddress host, int http, int ws, const char *root);
    void setOutputPrint(Print *debug);
    void setOnConnection(WebSocketConnectionCallback callback);
    void setOnData(WebSocketDataCallback callback);
    void begin();
    void loop();
    void sendTxt(uint8_t, const char *);

   private:
    void initSSDP(uint16_t port);
    void onGetFileList();
    String getFilePath(const String uri);
    bool getFileContent(const String uri);
    const char *getContentType(String filename);
    void onFileUploading();

    void socketEvent(uint8_t num, WStype_t type, uint8_t *payload,
                     size_t lenght);
    bool sendFile(const String);

    void onNotFound();
    void onNoContent();

    ESP8266WebServer *server;
    WebSocketsServer *socket;
    SSDPClass *ssdp;

    WebSocketConnectionCallback onConnection;
    WebSocketDataCallback onData;

    Print *output;
    const char *_root;

    void _print(const char *);
    void _println(const char * = NULL);
};
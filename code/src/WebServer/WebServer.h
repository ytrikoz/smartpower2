#pragma once

#include <Arduino.h>

// #include "index.html.gz.h"

class WebServerHandler {
   public:
    virtual void onConnection(const uint32_t num, const bool connected) = 0;
    virtual void onData(const uint32_t num, const String& data) = 0;
    virtual bool getResponse(const String& uri, String& body) = 0;
    virtual bool uriExist(const String& uri, String& lastModified) = 0;
};

class WebServer {
   public:
    virtual void init(WebServerHandler* h) {
        handler_ = h;
    }
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual void loop() = 0;

   public:
    virtual void sendData(const uint32_t, const String&) = 0;
    virtual void sendEvent(const String&, const String&) = 0;

   protected:
    WebServerHandler* handler_;
};
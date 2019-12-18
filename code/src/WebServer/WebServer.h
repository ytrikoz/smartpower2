#pragma once

#include <Arduino.h>

#include "index.html.gz.h"

typedef std::function<void(uint8_t)> WebServerConnectionEventHandler;
typedef std::function<void(uint8_t, const String&)> WebServerDataEventHandler;

class WebServer {
   public:
    virtual bool start();
    virtual void stop();
    virtual void loop();

   public:
    virtual void sendData(uint8_t, const char*) = 0;
    virtual void sendData(uint8_t, const String&) = 0;
    virtual void sendEvent(const String& content, const String& name) = 0;

   public:
    virtual void setOnConnection(WebServerConnectionEventHandler) = 0;
    virtual void setOnDisconnection(WebServerConnectionEventHandler) = 0;
    virtual void setOnReceiveData(WebServerDataEventHandler) = 0;
};

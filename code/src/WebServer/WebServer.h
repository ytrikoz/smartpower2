#pragma once

#include <Arduino.h>

#include "index.html.gz.h"

typedef std::function<void(const uint32_t num, const bool connected)> WebClientConnectionEventHandler;
typedef std::function<void(const uint32_t num, const String& data)> WebClientDataEventHandler;

class WebServer {
   public:
    virtual bool start() = 0;
    virtual void stop() = 0;
    virtual void loop() = 0;

   public:
    virtual void sendData(const uint32_t, const String&) = 0;
    virtual void sendEvent(const String&, const String&) = 0;
    virtual void setOnConnection(WebClientConnectionEventHandler) = 0;
    virtual void setOnData(WebClientDataEventHandler) = 0;
};

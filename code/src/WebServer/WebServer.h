#pragma once

#include <Arduino.h>

#include "WebHandler.h"

// #include "index.html.gz.h"

class WebServer {
   public:
    virtual void setHandler(WebServerHandler* h) {
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

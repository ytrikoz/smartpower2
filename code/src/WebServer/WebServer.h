#pragma once

#include <Arduino.h>

#include "graph.html.gz.h"
#include "index.html.gz.h"

typedef std::function<void(uint8_t)> WebConnectionEventHandler;
typedef std::function<void(uint8_t, String)> WebReceiveDataEventHandler;

class WebServer {
   public:
    virtual bool start();
    virtual void stop();
    virtual void loop();

    virtual void sendData(const uint8_t, const String &) = 0;
    virtual void setOnConnection(WebConnectionEventHandler) = 0;
    virtual void setOnDisconnection(WebConnectionEventHandler) = 0;
    virtual void setOnReceiveData(WebReceiveDataEventHandler) = 0;    
};
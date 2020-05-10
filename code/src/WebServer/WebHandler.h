#pragma once

#include <Arduino.h>

class WebServerHandler {
   public:
    virtual void onConnection(const uint32_t num, const bool connected) {

    };
    virtual void onData(const uint32_t num, const String& data) {

    };
    virtual bool getResponse(const String& uri, String& body) {
        return false;
    };
    virtual bool exists(const String& uri, String& lastModified) {
        return false;
    }
};
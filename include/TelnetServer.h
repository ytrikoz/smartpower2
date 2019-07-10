#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "consts.h"
#include "str_utils.h"
#include "ip_utils.h"

typedef std::function<bool(Stream*)> TelnetConnectEventHandler;
typedef std::function<void()> TelnetDisconnectEventHandler;

class TelnetServer
{
public:
    TelnetServer();
    void setOutput(Print *p);

    void begin(uint16_t port);
    void write(const char *);    
    void loop();
 
    bool hasClientConnected();
    void setOnConnection(TelnetConnectEventHandler eventHandler);
    void setOnDisconnect(TelnetDisconnectEventHandler eventHandler);          
private:
    #ifdef DEBUG_TELNET
    HardwareSerial *debug = &USE_DEBUG_SERIAL;
    #endif
    void start();
    void stop();

    void onConnect();
    void onDisconnect();
    
    bool active;
    bool connected;   
    uint16_t port;
    
    WiFiClient client;    
    WiFiServer *server;
    
    TelnetConnectEventHandler onConnectEvent;
    TelnetDisconnectEventHandler onDisconnectEvent;

    WiFiEventHandler onDisconnected, onGotIp;

    Print *output = &USE_SERIAL;
};
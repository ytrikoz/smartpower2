#include "TelnetServer.h"

TelnetServer::TelnetServer(uint16_t port) {
    this->port = port;

    onDisconnected = WiFi.onStationModeDisconnected(
        {[this](const WiFiEventStationModeDisconnected &event) {
            stop();
        }});
    onGotIp = WiFi.onStationModeGotIP(
        {[this](const WiFiEventStationModeGotIP &event) { begin(); }});

    active = false;
    initialized = false;
    connected = false;
}

void TelnetServer::setOutput(Print *p) { output = p; }

void TelnetServer::begin() {
    if (!initialized) {
        init();
    }    
    active = true;
}

void TelnetServer::stop() {
    output->printf_P(str_telnet);    
    output->printf_P(str_stopped);    
    active = false;
}

void TelnetServer::init() {
    output->printf_P(str_telnet);    
    output->printf_P(str_start);  
    output->printf_P(strf_port, port);    

    server = new WiFiServer(port);
    server->setNoDelay(true);
    server->begin();
    
    initialized = server->status() != CLOSED;
    
    if (initialized)
        output->printf_P(str_success);
    else
        output->printf_P(str_failed);  
    
    output->println();
}

bool TelnetServer::hasClientConnected() {
    return connected;
}

void TelnetServer::write(const char *payload) {
    if (hasClientConnected()) {
    #ifdef DEBUG_TELNET
        USE_DEBUG_SERIAL.printf_P(str_telnet);
        USE_DEBUG_SERIAL.printf_P(str_arrow_dest);
        USE_DEBUG_SERIAL.println(payload);
    #endif
        client.write(payload);
    }
}

void TelnetServer::loop() {
    if (!active)
        return;

    if (server->hasClient()) {
        if (!client) {
            client = server->available();
        } else {
            if (!client.connected()) {
                server->stop();
                client = server->available();
            } else {
                WiFiClient reject;
                reject = server->available();
                reject.write("connection is busy");
                reject.stop();
            }
        }
    }

    if (connected != hasClientConnected()) {
        connected = hasClientConnected();
        if (connected) {
            onConnect();
        } else {
            onDisconnect();
        }
    }
}

void TelnetServer::onConnect() {
    if (onConnectEvent) onConnectEvent(&client);
}

void TelnetServer::onDisconnect() {
    if (onDisconnectEvent) onDisconnectEvent();
}

void TelnetServer::setOnClientConnect(TelnetConnectEventHandler eventHandler) {
    onConnectEvent = eventHandler;
}

void TelnetServer::setOnCLientDisconnect(
    TelnetDisconnectEventHandler eventHandler) {
    onDisconnectEvent = eventHandler;
}
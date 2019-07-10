#include "TelnetServer.h"

TelnetServer::TelnetServer() { connected = false; }

void TelnetServer::setOutput(Print *p) { output = p; }

void TelnetServer::begin(uint16_t port) {
    this->port = port;
    onDisconnected = WiFi.onStationModeDisconnected(
        {[this](const WiFiEventStationModeDisconnected &event) {
            output->println("[telnet] stopped");
            active = false;
        }});

    onGotIp = WiFi.onStationModeGotIP(
        {[this](const WiFiEventStationModeGotIP &event) { start(); }});
}

void TelnetServer::start() {
    if (!active) {
        server = new WiFiServer(port);
        server->setNoDelay(true);
        server->begin();
        active = server->status() != CLOSED;
        if (active) {
            output->printf("[telnet] %s:%d\r\n", hostIP().toString().c_str(),
                           port);
        } else {
            output->println("[telnet] failed");
        }
    }
}

bool TelnetServer::hasClientConnected() { return client && client.connected(); }

void TelnetServer::write(const char *payload) {
    if (hasClientConnected()) {
#ifdef DEBUG_TELNET
        debug->print("[telnet]");
        debug->print(" -> ");
        debug->println(payload);
#endif

        client.write(payload);
    }
}

void TelnetServer::setOnConnection(TelnetConnectEventHandler eventHandler) {
    onConnectEvent = eventHandler;
}

void TelnetServer::setOnDisconnect(TelnetDisconnectEventHandler eventHandler) {
    onDisconnectEvent = eventHandler;
}

void TelnetServer::loop() {
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
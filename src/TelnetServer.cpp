#include "TelnetServer.h"

TelnetServer::TelnetServer() { connected = false; }

void TelnetServer::setOutput(Print *p) { output = p; }

void TelnetServer::begin(uint16_t port) {
    this->port = port;
    onDisconnected = WiFi.onStationModeDisconnected(
        {[this](const WiFiEventStationModeDisconnected &event) { stop(); }});

    onGotIp = WiFi.onStationModeGotIP(
        {[this](const WiFiEventStationModeGotIP &event) { start(); }});
}

void TelnetServer::start() {
    if (!active) {
        
        server = new WiFiServer(port);
        server->setNoDelay(true);
        server->begin();
        active = server->status() != CLOSED;        

        output->print(FPSTR(str_telnet));
        if (active) {
            output->printf_P(strf_ip_port, hostIP().toString().c_str(), port);
        } else {
            output->print(FPSTR(str_failed));
        }
        output->println();
    }
}

void TelnetServer::stop() {
    if (active) {
        server->stop();
        output->print(str_telnet);
        output->printf_P(str_stopped);
        output->println();
        active = false;
    }
}

bool TelnetServer::hasClientConnected() {
    return active && client && client.connected();
}

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
    if (!active) return;

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
                reject.println(F("already in use"));
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
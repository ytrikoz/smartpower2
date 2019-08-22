#include "TelnetServer.h"

TelnetServer::TelnetServer(uint16_t port)
    : port(port), active(false), initialized(false), connected(false) {
    staDisconnected = WiFi.onStationModeDisconnected(
        {[this](const WiFiEventStationModeDisconnected &event) { stop(); }});
    staGotIP = WiFi.onStationModeGotIP(
        {[this](const WiFiEventStationModeGotIP &event) { begin(); }});
}

void TelnetServer::setOutput(Print *p) { output = p; }

void TelnetServer::begin() {
    if (!initialized) {
        output->print(getSquareBracketsStrP(str_telnet));
        output->printf_P(strf_s_d, "*", port);
        output->println();
        
        server = new WiFiServer(port);
        server->setNoDelay(true);
        server->begin();

        initialized = server->status() != CLOSED;

        if (!initialized) {
            output->print(getSquareBracketsStrP(str_telnet));
            output->print(getStrP(str_failed));
        }
    }
    active = initialized;
}

void TelnetServer::stop() {
    if (active) {
        server->stop();
        active = false;        
        output->print(getSquareBracketsStrP(str_telnet));
        output->println(getStrP(str_stopped));
    }
}

bool TelnetServer::hasClientConnected() {
    return initialized & client & client.connected();
}

void TelnetServer::write(const char *payload) {
    if (hasClientConnected()) {
#ifdef DEBUG_TELNET
        DEBUG.printf_P(str_telnet);
        DEBUG.printf_P(str_arrow_dest);
        DEBUG.println(payload);
#endif
        client.write(payload);
    }
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
                reject.write(getStrP(msg_connection_is_busy, false).c_str());
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
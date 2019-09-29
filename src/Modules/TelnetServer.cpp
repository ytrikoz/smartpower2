#include "Modules/TelnetServer.h"

TelnetServer::TelnetServer() : AppModule(MOD_TELNET) {
    port = TELNET_PORT;
    active = initialized = connected = false;
};

bool TelnetServer::begin() {
    if (!initialized) {
        output->print(StrUtils::getIdentStrP(str_telnet));
        output->println(port);

        server = new WiFiServer(port);
        server->begin();
        server->setNoDelay(true);

        initialized = server->status() != CLOSED;

        staDisconnected = WiFi.onStationModeDisconnected(
            {[this](const WiFiEventStationModeDisconnected &event) {
                stop();
            }});
        staGotIP = WiFi.onStationModeGotIP(
            {[this](const WiFiEventStationModeGotIP &event) { begin(); }});

        if (!initialized) {
            output->print(StrUtils::getIdentStrP(str_telnet));
            output->print(StrUtils::getStrP(str_failed));
        }
    }
    active = initialized;

    return active;
}

void TelnetServer::stop() {
    if (active) {
        output->print(StrUtils::getIdentStrP(str_telnet));
        output->println(StrUtils::getStrP(str_stopped));
        if (hasClientConnected()) client.stop();
        server->stop();
        active = false;
    }
}

bool TelnetServer::hasClientConnected() {
    return active && client && client.connected();
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
                reject.write(
                    StrUtils::getStrP(msg_connection_is_busy, false).c_str());
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
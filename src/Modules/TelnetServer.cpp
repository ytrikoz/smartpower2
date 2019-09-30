#include "Modules/TelnetServer.h"

TelnetServer::TelnetServer() : AppModule(MOD_TELNET) {
    port = TELNET_PORT;
    out->print(StrUtils::getIdentStrP(str_telnet));
    out->print(StrUtils::getStrP(str_port));
    out->print(": ");
    out->println(port);

    server = new WiFiServer(port);
    active = initialized = connected = false;
};

bool TelnetServer::begin() {
    start();
    return active;
}

void TelnetServer::start() {
    server->begin();
    server->setNoDelay(true);

    active = server->status() != CLOSED;

    if (!active) {
        out->print(StrUtils::getIdentStrP(str_telnet));
        out->println(StrUtils::getStrP(str_failed));
    }
}

void TelnetServer::stop() {
    if (active) {
        out->print(StrUtils::getIdentStrP(str_telnet));
        out->println(StrUtils::getStrP(str_stopped));
        if (hasClientConnected())
            client.stop();
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
    if (onConnectEvent)
        onConnectEvent(&client);
}

void TelnetServer::onDisconnect() {
    if (onDisconnectEvent)
        onDisconnectEvent();
}

void TelnetServer::setOnClientConnect(TelnetConnectEventHandler eventHandler) {
    onConnectEvent = eventHandler;
}

void TelnetServer::setOnCLientDisconnect(
    TelnetDisconnectEventHandler eventHandler) {
    onDisconnectEvent = eventHandler;
}
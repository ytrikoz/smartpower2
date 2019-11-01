#include "Modules/TelnetServer.h"

using namespace PrintUtils;

TelnetServer::TelnetServer() : AppModule(MOD_TELNET) {
    active = connected = false;
};

void TelnetServer::setConfig(Config *cfg) { port = TELNET_PORT; }

bool TelnetServer::begin() {
    print_ident(out, FPSTR(str_telnet));
    if (!server)
        server = new WiFiServer(port);
    server->begin();
    server->setNoDelay(true);
    active = (server->status() != CLOSED);
    if (active)
        println_nameP_value(out, str_port, port);
    else
        println(out, FPSTR(str_failed));
    return active;
}

void TelnetServer::end() {
    if (active) {
        if (hasClient())
            client.stop();
        server->stop();
        active = false;
        say_strP(str_stopped);
    }
}

bool TelnetServer::hasClient() { return active && client.connected(); }

void TelnetServer::write(const char *payload) {
    if (hasClient()) {
#ifdef DEBUG_TELNET
        DEBUG.printf_P(str_telnet);
        DEBUG.printf_P(str_arrow_dest);
        DEBUG.println(payload);
#endif
        client.write(payload);
    }
}

void TelnetServer::setEventHandler(TelnetEventHandler h) { eventHandler = h; }

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
                WiFiClient rejected;
                rejected = server->available();
                rejected.write_P(msg_connection_busy, 32);
                rejected.stop();
            }
        }
    }

    if (connected != hasClient()) {
        connected = hasClient();
        if (connected) {
            onConnect();
        } else {
            onDisconnect();
        }
    }
}

void TelnetServer::onConnect() {
    print_ident(out, FPSTR(str_telnet));
    print_strP_var(out, str_client, FPSTR(str_connected));

    if (eventHandler)
        eventHandler(CLIENT_CONNECTED, &client);
}

void TelnetServer::onDisconnect() {
    print_ident(out, FPSTR(str_telnet));
    print_strP_var(out, str_client, FPSTR(str_disconnected));

    if (eventHandler)
        eventHandler(CLIENT_DISCONNECTED, &client);
}

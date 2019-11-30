#include "Modules/TelnetServer.h"

using namespace PrintUtils;
using namespace StrUtils;

bool TelnetServer::onInit() {
    server_ = new WiFiServer(port_);
    return server_;
}

bool TelnetServer::onStart() {
    server_->begin();
    server_->setNoDelay(true);
    return server_->status() != CLOSED;
}

void TelnetServer::onStop() {
    if (hasClient())
        client_.stop();
    server_->stop();
}

bool TelnetServer::hasClient() { return client_.connected(); }

void TelnetServer::write(const char* payload) {
    if (hasClient())
        client_.write(payload);
}

void TelnetServer::setEventHandler(TelnetEventHandler h) { eventHandler_ = h; }

void TelnetServer::onLoop() {
    if (server_->hasClient()) {
        if (!client_) {
            client_ = server_->available();
        } else {
            if (!client_.connected()) {
                server_->stop();
                client_ = server_->available();
            } else {
                WiFiClient rejected;
                rejected = server_->available();
                rejected.write_P(msg_connection_busy, 32);
                rejected.stop();
            }
        }
    }

    if (lastConnected_ != hasClient()) {
        lastConnected_ = hasClient();
        if (lastConnected_) {
            onConnect();
        } else {
            onDisconnect();
        }
    }
}

void TelnetServer::onConnect() {
    print_ident(out, FPSTR(str_telnet));

    if (eventHandler_)
        eventHandler_(CLIENT_CONNECTED, &client_);
}

void TelnetServer::onDisconnect() {
    print_ident(out, FPSTR(str_telnet));
    println(out, FPSTR(str_disconnected));

    if (eventHandler_)
        eventHandler_(CLIENT_DISCONNECTED, &client_);
}

size_t TelnetServer::printDiag(Print* p) {
    size_t n = print_paramP_value(p, str_port, port_);
    n = +print_paramP_value(p, str_connected, getBoolStr(hasClient()));
    if (hasClient()) {
        n = +print_paramP_value(
            out, str_client,
            fmt_ip_port(client_.remoteIP(), client_.remotePort()).c_str());
    }

    return n;
}

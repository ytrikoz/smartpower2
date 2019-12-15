#include "Modules/Telnet.h"

#include "CommandShell.h"

using namespace PrintUtils;
using namespace StrUtils;

namespace Modules {

bool Telnet::onInit() {
    server_ = new WiFiServer(port_);
    
    term_.enableControlCodes();
    term_.enableEcho(false);

    return true;
}

void Telnet::setShell(CommandShell* shell) {
    shell_ = shell;
    shell_->setTerm(&term_);
}

bool Telnet::onStart() {
    server_->begin();
    server_->setNoDelay(true);
    return server_->status() != CLOSED;
}

void Telnet::onStop() {
    if (hasClient())
        client_.stop();
    server_->stop();
}

bool Telnet::hasClient() { return client_.connected(); }

void Telnet::sendData(const String& data) {
    if (hasClient())
        client_.write(data.c_str());
}

void Telnet::setEventHandler(TelnetEventHandler h) { eventHandler_ = h; }

void Telnet::onLoop() {
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
                rejected.write_P(str_busy, 32);
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
    
     if(shell_) shell_->loop();
}

void Telnet::onConnect() {    
    shell_->term()->setStream(&client_);
    if (eventHandler_)
        eventHandler_(CLIENT_CONNECTED, &client_);
}

void Telnet::onDisconnect() {
    shell_->term()->setStream(nullptr);
    if (eventHandler_)
        eventHandler_(CLIENT_DISCONNECTED, nullptr);
}

void Telnet::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_connected)] = hasClient();
    if (hasClient()) {
        doc[FPSTR(str_client)] = prettyIpAddress(client_.remoteIP(), client_.remotePort());
        doc[FPSTR(str_shell)] = shell_->isOpen();
    }
}

}
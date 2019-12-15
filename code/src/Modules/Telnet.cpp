#include "Modules/Telnet.h"

using namespace PrintUtils;
using namespace StrUtils;

namespace Modules {

bool Telnet::onInit() {
    server_ = new WiFiServer(port_);
    
    terminal_ = new Terminal(&client_);
    terminal_->enableControlCodes();
    terminal_->enableEcho(false);
    terminal_->setOnEvent([this](TerminalEventEnum event, Stream* console) {
        if (terminalHandler_) terminalHandler_(event, console); 
    });

    return true;
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

Terminal* Telnet::getTerminal() {
    return terminal_;
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

    if(client_.connected()) {
        terminal_->loop();
    }
}

void Telnet::onConnect() {    
    if (eventHandler_)
        eventHandler_(CLIENT_CONNECTED, &client_);
}

void Telnet::onDisconnect() {
    if (eventHandler_)
        eventHandler_(CLIENT_DISCONNECTED, nullptr);
}

size_t Telnet::onDiag(Print* p) {
    DynamicJsonDocument doc(128);
    doc[FPSTR(str_port)] = port_;
    doc[FPSTR(str_connected)] = hasClient();
    if (hasClient())
        doc[FPSTR(str_client)] = prettyIpAddress(client_.remoteIP(), client_.remotePort());
    return serializeJson(doc, *p);
}

}
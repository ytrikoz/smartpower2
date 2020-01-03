#pragma once

#include <Arduino.h>

#include "Core/Logger.h"
#include "Core/Module.h"

#include <ESP8266WiFi.h>
#include <IPAddress.h>
#include <WiFiUdp.h>

class UdpPacket : public StringPusher {
   public:
    UdpPacket() { udp_ = new WiFiUDP(); };

    ~UdpPacket() {
        if (udp_) {
            udp_->stop();
            delete udp_;
        }
    }
    void setup(const IPAddress ip, const uint16_t port) {
        ip_ = ip;
        port_ = port;
    };

    IPAddress ip() {
        return ip_;
    }

    uint16_t port() {
        return port_;
    }

    bool push(const String& data) override {
        bool res = false;
        if (udp_ && udp_->beginPacket(ip_, port_)) {
            udp_->print(data);
            res = udp_->endPacket();
        }
        return res;
    };

   private:
    WiFiUDP* udp_;
    IPAddress ip_;
    uint16_t port_;
};

class VisualSyslogServer {
   public:
    VisualSyslogServer(UdpPacket* dest) : dest_(dest), facility_(1){};

    void setFacility(int8_t facility) {
        facility_ = facility;
    }

    bool send(LogSeverity level, const char* tag, const char* message) {
        bool res = false;
        String buf;
        buf += '<';
        buf += getMask(level);
        buf += '>';
        buf += APP_NAME;
        buf += ' ';
        buf += tag;
        buf += ':';
        buf += ' ';
        buf += message;
        if (dest_->push(buf)) {
            cnt_++;
            size_ += buf.length();
            res = true;
        }
        return res;
    }

    size_t cnt() {
        return cnt_;
    }

    size_t total() {
        return size_;
    }

   private:
    int getMask(LogSeverity level) {
        return (int)facility_ * 8 + (int)level;
    }

   private:
    UdpPacket* dest_;
    uint8_t facility_;
    size_t cnt_;
    size_t size_;
};

namespace Modules {

class Syslog : public Module, public Logger {
   public:
    Syslog();

   public:
    virtual bool log(const LogSeverity level, const char* module, const char* str) override;
    void setSource(StringPullable* source);

   protected:
    virtual bool onConfigChange(const ConfigItem param, const String& value) override;
    virtual bool onInit() override;
    virtual bool onStart() override;
    virtual void onLoop() override;
    virtual void onDiag(const JsonObject& doc) override;

   private:
    void setServer(const char* address, uint16_t port);

   private:
    char* name_;
    UdpPacket tranport_;
    VisualSyslogServer proto_;
    StringPullable* source_;
};

}  // namespace Modules

/* Syslog Facility
0	kernel messages
1	user-level messages
2	mail system
3	system daemons
4	security/authorization messages
5	messages generated internally by syslog
6	line printer subsystem
7	network news subsystem
8	UUCP subsystem
9	clock daemon
10	security/authorization messages
11	FTP daemon
12	NTP subsystem
13	log audit
14	log alert
15	clock daemon
16	local use 0 (local0)
17	local use 1 (local1)
18	local use 2 (local2)
19	local use 3 (local3)
20	local use 4 (local4)
21	local use 5 (local5)
22	local use 6 (local6)
23	local use 7 (local7)
*/

/* Syslog Severity
0	EMERGENCY       A "panic" condition
1	ALERT           Should be corrected immediately
2	CRITICAL        Should be corrected immediately, indicates failure in a
primary system
3	ERROR	        Non-urgent failures
4	WARNING         Warning messages - not an error, but indication that an
error will occur if action is not taken 6	INFORMATIONAL   Normal
operational messages - no action required. 7	DEBUG	        Info useful to
developers for debugging, not useful during operations.
*/
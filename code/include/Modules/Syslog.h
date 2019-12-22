#pragma once

#include <Arduino.h>

#include "Core/Module.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

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
#define SYSLOG_FACILITY 1

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
enum SysLogSeverity { SYSLOG_ALERT = 1,
                      SYSLOG_INFO = 6,
                      SYSLOG_DEBUG = 7 };
namespace Modules {


class Syslog : public Module {
   public:
    Syslog(uint16_t port):Module(), udp_(nullptr), ip_(IPADDR_NONE), port_(port) {};

   public:
    void alert(const String &, const String &);
    void info(const String &, const String &);
    void debug(const String &, const String &);
    void onDiag(const JsonObject &doc) override;

   protected:
    bool onInit() override;
    bool onStart() override;
    void onStop() override;
    void onLoop() override;

   private:
    const char *getHostname();

   private:
    void send(const SysLogSeverity level, const String &routine, const String &message);
    void printPacket(const SysLogSeverity level, const String &routine,
                     const String &message);

   private:
    WiFiUDP *udp_;
    IPAddress ip_;
    uint16_t port_;
};

}  // namespace Modules
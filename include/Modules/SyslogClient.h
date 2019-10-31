#pragma once

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "AppModule.h"

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
#define SYSLOG_FACILITY 16

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
enum SysLogSeverity { SYSLOG_ALERT = 1, SYSLOG_INFO = 6, SYSLOG_DEBUG = 7 };

class SyslogClient : public AppModule {
  public:
    SyslogClient();
    SyslogClient(WiFiUDP *upd);
    void setConfig(Config *cfg);

  public:
    void alert(String str);
    void info(String str);
    void debug(String str);

  private:
    void setServer(const char *str);
    void setPort(const uint16_t udp_port);
    void send(SysLogSeverity level, String message);
    String getPayload(const SysLogSeverity level, unsigned long time,
                      const char *host, const char *message);

  private:
    WiFiUDP *udp;
    char *server;
    uint16_t port;
};

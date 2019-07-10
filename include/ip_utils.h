#pragma once
#include <ESP8266WiFi.h>

#include "str_utils.h"

String macFormated(uint8 hwaddr[6]);
String getFormatedSocketStr(IPAddress ip, int port);

IPAddress atoip(const char* str);
String iptoa(IPAddress ipaddr);
bool validIPStr(const char* input);

IPAddress hostIP();

String hostSSID();

bool scanNetworks(const char* ssid);

void setupAP(IPAddress);

bool startAP(const char* ssid, const char* password, IPAddress host);

void startSTA(const char* ssid, const char* password, IPAddress host,
              IPAddress subnet, IPAddress gateway, IPAddress dns);
void startSTA(const char* ssid, const char* passwd);

void setupSTA(IPAddress host, IPAddress gateway, IPAddress subnet,
              IPAddress dns);
void setupSTA(const char* ssid, const char* password, IPAddress ipaddr,
              IPAddress subnet, IPAddress gateway, IPAddress dns);
void setupSTA(const char* ssid, const char* password);
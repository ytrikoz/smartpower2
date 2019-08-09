#pragma once

#include <Arduino.h>
#include "IPAddress.h"
#include "types.h"
#include "debug.h"

typedef std::function<void(bool hasNetwork)> NetworkStateChangeEventHandler;

namespace wireless {

WirelessMode getWirelessMode();
bool hasNetwork();
String hostSSID();
IPAddress hostIP();
String hostIPInfo();
String hostName();
String RSSIInfo();
String hostAPPassword();
void printDiag(Print* p);

void start_wifi();
void setOnNetworkStateChange(NetworkStateChangeEventHandler eventHandler);
void updateState();
void setNetworkState(NetworkState value);
void onNetworkUp();
void onNetworkDown();

bool scanNetworks(const char* ssid);

bool disconnectWiFi();

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

void useStaticStationIP(bool enabled);

}  // namespace wireless

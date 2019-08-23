#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include "CommonTypes.h"

typedef std::function<void(bool hasNetwork)> NetworkStateChangeEventHandler;

namespace Wireless {

String getConfigHostname();
String getConnectionStatus();
WirelessMode getWirelessMode();
bool hasNetwork();
String hostSSID();
IPAddress hostIP();
String wifiModeInfo();
String hostIPInfo();
String hostName();
String RSSIInfo();
String hostSTA_SSID();
String hostAP_Password();
String hostAP_SSID();

IPAddress hostAP_IP();
IPAddress hostSTA_IP();

void printDiag(Print* p);

void start_wifi();
void setOnNetworkStateChange(NetworkStateChangeEventHandler eventHandler);
void updateState();
void setNetworkState(NetworkState value);
void onNetworkUp();
void onNetworkDown();

bool scanNetworks(const char* ssid);
bool disconnectWiFi();

void setMode(WirelessMode);
void setupAP(IPAddress);
bool startAP(const char* ssid, const char* password);

void setupSTA();
void setupSTA(IPAddress ipaddr, IPAddress subnet, IPAddress gateway,
              IPAddress dns);
bool startSTA(const char* ssid, const char* password);

void useStaticStationIP(bool enabled);

}  // namespace Wireless

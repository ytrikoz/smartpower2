#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include "CommonTypes.h"

typedef std::function<void(bool hasNetwork, unsigned long time)> NetworkStatusChangeEventHandler;

namespace Wireless {

enum Mode { WLAN_OFF = 0, WLAN_STA = 1, WLAN_AP = 2, WLAN_AP_STA = 3 };

enum NetworkStatus { NETWORK_DOWN, NETWORK_UP };

String getConfigHostname();
String getConnectionStatus();
Mode getMode();
bool hasNetwork();
String hostSSID();
IPAddress hostIP();
String wifiModeInfo();
String networkStateInfo();
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
void setOnNetworkStatusChange(NetworkStatusChangeEventHandler);
void updateState();
void setNetworkStatus(NetworkStatus);
void onNetworkUp();
void onNetworkDown();
void setBroadcastTo(uint8_t);
bool scanNetworks(const char* ssid);
bool disconnectWiFi();

void changeMode(Mode mode);
void setupAP(IPAddress);
bool startAP(const char* ssid, const char* password);

void setupSTA();
void setupSTA(IPAddress ipaddr, IPAddress subnet, IPAddress gateway,
              IPAddress dns);
bool startSTA(const char* ssid, const char* password);

void useStaticStationIP(bool enabled);

}  // namespace Wireless

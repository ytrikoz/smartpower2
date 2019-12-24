#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include <ESP8266WiFi.h>
#include <user_interface.h>

#include "BuildConfig.h"
#include "ConfigHelper.h"
#include "SysInfo.h"

namespace Wireless {

typedef std::function<void(bool has, unsigned long time)> NetworkStatusChangeEventHandler;

void setOutput(Print *p);
void start();
void init(NetworkMode mode, const char *host, uint8_t tpw);
void setMode(NetworkMode mode);
void setOnNetworkStatusChange(NetworkStatusChangeEventHandler);
void updateState();
void setNetworkStatus(NetworkStatus);
void onNetworkUp();
void onNetworkDown();
bool hasNetwork();

NetworkMode getMode();
String getModeStr(NetworkMode mode);

String wifiPhyMode();
String wifiChannel();
String getSTAStatus();
String getNetworkState();

String hostSSID();
String hostSTA_SSID();
IPAddress hostSTA_IP();
String hostSTA_RSSI();

String hostName();
IPAddress hostIP();
IPAddress hostSubnet();
IPAddress hostGateway();
IPAddress hostDNS();

String hostMac();

String AP_Name();
String AP_Password();
String AP_SSID();
IPAddress AP_IP();
uint8_t AP_Clients();

void setupAP(const IPAddress host);
bool startAP(const char *ssid, const char *password);

void setupSTA();
void setupSTA(const IPAddress ipaddr, const IPAddress subnet,
              const IPAddress gateway, const IPAddress dns);
bool startSTA(const char *ssid, const char *password);

void setBroadcast(uint8_t);
bool disconnectWiFi();

bool isScanning();

void startWiFiScan(bool hidden);
}  // namespace Wireless

void onScanComplete(int found);

void useStaticStationIP(bool enabled);
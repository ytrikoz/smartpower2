#pragma once

#include <Arduino.h>
#include <IPAddress.h>

#include "CommonTypes.h"

typedef std::function<void(bool has, unsigned long time)>
    NetworkStatusChangeEventHandler;

namespace Wireless {

enum NetworkMode {
    NETWORK_OFF = 0,
    NETWORK_STA = 1,
    NETWORK_AP = 2,
    NETWORK_AP_STA = 3
};

enum NetworkStatus { NETWORK_DOWN = 0,
                     NETWORK_UP = 1 };

String getWiFiPhyMode();

String getWifiChannel();

String hostSTA_StatusStr();

NetworkMode getMode();

bool hasNetwork();

String networkStateInfo();

String hostSSID();

IPAddress hostDNS();

const IPAddress hostIP();

IPAddress hostGateway();

IPAddress hostSubnet();

String hostMac();

String hostIPInfo();

const String hostName();

String hostSTA_SSID();

String hostAP_Name();

String hostAP_Password();

String hostAP_SSID();

IPAddress hostAP_IP();

IPAddress hostSTA_IP();

String hostSTA_RSSI();

size_t printDiag(Print *p);

void start();

void init(NetworkMode mode, const char *host, uint8_t tpw);

void setMode(NetworkMode mode);

void setOnNetworkStatusChange(NetworkStatusChangeEventHandler);

void updateState();

void setNetworkStatus(NetworkStatus);

void onNetworkUp();

void onNetworkDown();

bool scanNetworks(const char *ssid);

bool disconnectWiFi();

void setupAP(const IPAddress host);

bool startAP(const char *ssid, const char *password);

void setupSTA();

void setupSTA(const IPAddress ipaddr, const IPAddress subnet,
              const IPAddress gateway, const IPAddress dns);

bool startSTA(const char *ssid, const char *password);

void println_mode(Print *p, NetworkMode mode);

String getModeStr(NetworkMode mode);

void useStaticStationIP(bool enabled);

void setBroadcast(uint8_t _new);

uint8_t getAPClients();

}  // namespace Wireless

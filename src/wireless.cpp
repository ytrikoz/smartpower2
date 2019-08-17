#include "Wireless.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <user_interface.h>

#include "BuildConfig.h"
#include "ConfigHelper.h"
#include "Global.h"
#include "SysInfo.h"

namespace Wireless {

WirelessMode mode = WLAN_OFF;
NetworkState network = NETWORK_DOWN;
unsigned long lastNetworkUp = 0;
unsigned long lastNetworkDown = 0;

WiFiEventHandler staGotIpEventHandler, staConnectedEventHandler,
    staDisconnectedEventHandler;

NetworkStateChangeEventHandler onNetworkStateChange;

String getConfigHostname() {
    char buf[32];
    strcpy_P(buf, HOST_NAME);
    return String(buf);
}

String getConnectionStatus() {
    char buf[32];
    memset(buf, 0, sizeof(char) * 32);
    station_status_t status = wifi_station_get_connect_status();
    switch (status) {
        case STATION_IDLE:
            strcpy_P(buf, str_idle);
            break;
        case STATION_CONNECTING:
            strcpy_P(buf, str_connecting);
            break;
        case STATION_WRONG_PASSWORD:
            strcpy_P(buf, str_wrong);
            strcat_P(buf, str_password);
            break;
        case STATION_NO_AP_FOUND:
            strcpy_P(buf, str_no);
            strcat_P(buf, str_ap);
            strcat_P(buf, str_found);
            break;
        case STATION_CONNECT_FAIL:
            strcpy_P(buf, str_connection);
            strcat_P(buf, str_failed);
            break;
        case STATION_GOT_IP:
            strcpy_P(buf, str_connected);
            strcat_P(buf, str_got);
            strcat_P(buf, str_ip);
            break;
    }
    return String(buf);
}

void printDiag(Print *p) {
    p->print(FPSTR(str_wifi));
    p->printf_P(strf_mode, mode);
    p->print(FPSTR(str_network));
    p->print(network);
    p->println();

    WiFi.printDiag(*p);
}

IPAddress hostAP_IP() { return WiFi.softAPIP(); }

IPAddress hostSTA_IP() { return WiFi.localIP(); }

String hostAP_Password() { return WiFi.softAPPSK(); }

String hostAP_SSID() { return WiFi.softAPSSID(); }

String hostSTA_SSID() { return WiFi.SSID(); }

String hostSSID() {
    String str;
    switch (getWirelessMode()) {
        case WLAN_STA:
        case WLAN_AP_STA:
            str = WiFi.SSID();
            break;
        case WLAN_AP:
            str = WiFi.softAPSSID();
            break;
        case WLAN_OFF:
            str = "";
            break;
        default:
            break;
    }
    return str;
}

IPAddress hostIP() {
    IPAddress ip;
    switch (getWirelessMode()) {
        case WLAN_OFF:
            break;
        case WLAN_STA:
        case WLAN_AP_STA:
            ip = WiFi.localIP();
            break;
        case WLAN_AP:
            ip = WiFi.softAPIP();
            break;
    }
    return ip;
}

String hostName() {
    String str;
    switch (getWirelessMode()) {
        case WLAN_STA:
        case WLAN_AP_STA:
            str = WiFi.hostname();
            break;
        case WLAN_OFF:
        case WLAN_AP:
            str = getConfigHostname();
            break;
    }
    return str;
}

void setupAP(IPAddress ip) {
    WiFi.mode(WIFI_AP);
    WiFi.disconnect();
    delay(100);
    WiFi.softAPConfig(ip, ip, IPAddress(255, 255, 255, 0));
}

void setupSTA(IPAddress ip, IPAddress gateway, IPAddress subnet,
              IPAddress dns) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.config(ip, gateway, subnet, dns);
}

bool scanWiFi(const char *ssid) {
    setupSTA(hostIP(), IPAddress(IP_ADDR_ANY), IPAddress(IP_ADDR_ANY),
             IPAddress(IP_ADDR_ANY));

    int8_t discovered = WiFi.scanNetworks(false, true, 0, (uint8_t *)ssid);

    WiFi.scanDelete();
    WiFi.disconnect(true);
    delay(100);

    return discovered;
}

void startSTA(const char *ssid, const char *passwd) {
    startSTA(ssid, passwd, IPAddress(IP_ADDR_ANY), IPAddress(IP_ADDR_ANY),
             IPAddress(IP_ADDR_ANY), IPAddress(IP_ADDR_ANY));
}

void startSTA(const char *ssid, const char *password, IPAddress ip,
              IPAddress subnet, IPAddress gateway, IPAddress dns) {
    PRINT_WIFI_STA
    USE_SERIAL.print(FPSTR(str_arrow_dest));
    USE_SERIAL.print(ssid);
    if (ip.operator== IP4_ADDR_ANY) {
        USE_SERIAL.printf_P(str_dhcp_on);
    } else {
        PRINT_IP
    }
    USE_SERIAL.println();

    setupSTA(ip, gateway, subnet, dns);

    WiFi.begin(ssid, password);

    uint8_t result = WiFi.waitForConnectResult();
    if (result == WL_CONNECTED) {
        WiFi.setAutoConnect(true);
        WiFi.setAutoReconnect(true);
    } else {
        PRINT_WIFI_STA
        USE_SERIAL.println(FPSTR(str_failed));
    }
}

bool startAP(const char *ssid, const char *passwd, IPAddress ip) {
    bool result = false;
    setupAP(ip);
    bool unsec = strlen(passwd) < 8;
    if (unsec) {
        USE_SERIAL.printf_P(str_unsecured);
        result = WiFi.softAP(ssid);
    } else {
        result = WiFi.softAP(ssid, passwd);
    }
    return result;
}

boolean disconnectWiFi() { return WiFi.disconnect(); }

bool isActive() {
    return WiFi.softAPgetStationNum() || WiFi.status() == WL_CONNECTED;
}

void start_safe_wifi_ap() {
    char ap_ssid[32];
    strcpy(ap_ssid, APPNAME);
    strcat(ap_ssid, getChipId().c_str());
    char ap_passwd[] = "12345678";
    IPAddress ap_ipaddr = IPAddress(192, 168, 4, 1);

#ifdef DEBUG_WIRELESS
    PRINT_WIFI_AP
    PRINT_WIFI_AP_CONFIG
#endif

    if (startAP(ap_ssid, ap_passwd, ap_ipaddr)) {
        USE_SERIAL.printf_P(str_success);
    } else {
        USE_SERIAL.printf_P(str_failed);
    }
}

void start_wifi() {
    mode = (WirelessMode)config->getWiFiMode();
    uint8_t tpw = config->getTPW();
    system_phy_set_max_tpw(tpw);
    WiFi.hostname(Wireless::getConfigHostname());

#ifdef DEBUG_WIRELESS
    PRINTLN_WIFI_CONFIG
#endif

    if (mode == WLAN_AP) {
        const char *ap_ssid = config->getSSID_AP();
        const char *ap_passwd = config->getPassword_AP();
        IPAddress ap_ipaddr = config->getIPAddr_AP();

#ifdef DEBUG_WIRELESS
        PRINT_WIFI_AP
        PRINT_WIFI_AP_CONFIG
#endif

        bool result = startAP(ap_ssid, ap_passwd, ap_ipaddr);

#ifdef DEBUG_WIRELESS
        if (result)
            USE_SERIAL.printf_P(str_success);
        else
            USE_SERIAL.printf_P(str_failed);
        USE_SERIAL.println();
#endif

        if (result)
            setNetworkState(NETWORK_UP);
        else
            setNetworkState(NETWORK_DOWN);
    }

    if (mode == WLAN_STA) {
        staConnectedEventHandler = WiFi.onStationModeConnected(
            [](const WiFiEventStationModeConnected &e) {
                PRINTLN_WIFI_STA_CONNECTED
            });

        staGotIpEventHandler =
            WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &e) {
                IPAddress ip, subnet, gateway, dns;
                ip = e.ip;
                subnet = e.mask;
                gateway = e.gw;
                dns = WiFi.dnsIP();

                PRINTLN_WIFI_STA_GOT_IP

                updateState();
            });

        staDisconnectedEventHandler = WiFi.onStationModeDisconnected(
            [](const WiFiEventStationModeDisconnected &e) {
                if (network == NETWORK_UP) {
                    PRINTLN_WIFI_STA_DISCONNECTED
                }
                updateState();
            });

        const char *ssid = config->getSSID();
        const char *passwd = config->getPassword();
        const bool dhcp = config->getDHCP();
        if (dhcp) {
            startSTA(ssid, passwd);
        } else {
            IPAddress ip, subnet, gateway, dns;
            ip = config->getIPAddr();
            subnet = config->getNetmask();
            gateway = config->getGateway();
            dns = config->getDNS();
            startSTA(ssid, passwd, ip, subnet, gateway, dns);
        }
    }
}

void updateState() {
    switch (getWirelessMode()) {
        case WLAN_OFF:
            PRINTLN_WIFI_SWITCHED_OFF
            setNetworkState(NETWORK_DOWN);
            break;
        case WIFI_AP:
            // ping ?
            break;
        case WIFI_STA:
        case WIFI_AP_STA:
            if (WiFi.isConnected()) {
                setNetworkState(NETWORK_UP);
            } else {
                setNetworkState(NETWORK_DOWN);
            }
            break;
        default:
            break;
    }
}

void setOnNetworkStateChange(NetworkStateChangeEventHandler eventHandler) {
    onNetworkStateChange = eventHandler;
}

void setNetworkState(NetworkState value) {
    if (network != value) {
        network = value;
        if (network == NETWORK_UP) {
            onNetworkUp();
        } else if (network == NETWORK_DOWN) {
            onNetworkDown();
        }
    }
}

void onNetworkUp() {
    PRINTLN_WIFI_NETWORK_UP
    lastNetworkUp = millis();
    if (onNetworkStateChange) onNetworkStateChange(true);
    start_services();
}

void onNetworkDown() {
    PRINTLN_WIFI_NETWORK_DOWN
    lastNetworkDown = millis();
    if (onNetworkStateChange) onNetworkStateChange(false);
}

WirelessMode getWirelessMode() { return mode = (WirelessMode)WiFi.getMode(); }

bool hasNetwork() { return network == NETWORK_UP; }

String RSSIInfo() {
    int rssi = wifi_station_get_rssi();
    String str = String(rssi);
    str += "dB";
    return String(str);
}

// https://github.com/esp8266/Arduino/issues/4114
class WiFiStationStaticIP : public ESP8266WiFiSTAClass {
   public:
    void useStaticStationIP(bool enabled) { _useStaticIp = enabled; }
};

void enableStaticStationIP(bool enabled) {
    WiFiStationStaticIP tmp;
    tmp.useStaticStationIP(enabled);
}

String hostIPInfo() {
    char buf[OUTPUT_MAX_LENGTH];
    memset(&buf, 0, OUTPUT_MAX_LENGTH);
    strcpy_P(buf, str_wifi);
    char tmp[32];
    switch (getWirelessMode()) {
        case WLAN_OFF:
            strcpy_P(buf, str_switched);
            strcpy_P(buf, str_off);
            break;
        case WLAN_STA:
            sprintf_P(tmp, strf_ip, WiFi.localIP().toString().c_str());
            strcat_P(buf, tmp);
            break;
        case WLAN_AP:
            strcat_P(buf, str_ap);
            sprintf_P(tmp, strf_ip, WiFi.softAPIP().toString().c_str());
            strcat_P(buf, tmp);
            break;
        case WLAN_AP_STA:
            strcat_P(buf, str_sta);
            sprintf_P(tmp, strf_ip, WiFi.localIP().toString().c_str());
            strcat_P(buf, tmp);
            strcat_P(buf, " ");
            strcat_P(buf, str_ap);
            sprintf_P(tmp, strf_ip, WiFi.softAPIP().toString().c_str());
            strcat_P(buf, tmp);
    }
    strcat_P(buf, str_reconnect);
    strcat_P(buf, wifi_station_get_reconnect_policy() ? str_yes : str_no);
    return String(buf);
}  // namespace Wireless

}  // namespace Wireless

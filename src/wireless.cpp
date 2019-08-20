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
    station_status_t status = wifi_station_get_connect_status();
    String str;
    switch (status) {
        case STATION_CONNECTING:
            str = getStrP(str_connecting);
            break;
        case STATION_GOT_IP:
            str = getStrP(str_connected);
            break;
        case STATION_NO_AP_FOUND:
            str = getStrP(str_ap) + getStrP(str_not_found, false);
            break;
        case STATION_CONNECT_FAIL:
            str = getStrP(str_connection) + getStrP(str_failed, false);
            break;
        case STATION_WRONG_PASSWORD:
            str = getStrP(str_wrong) + getStrP(str_password, false);
            break;
        case STATION_IDLE:
            str = getStrP(str_idle);
            break;
        default:
            str = getStrP(str_disconnected);
            break;
    }
    return str;
}

void printDiag(Print *p) {
    p->print(getStrP(str_wifi));
    p->print(getStrP(str_mode));
    p->println(mode);

    p->print(getStrP(str_network));
    p->println(network);

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

void setMode(WirelessMode mode) {
    WiFi.mode((WiFiMode_t)mode);
    WiFi.disconnect();
    delay(100);
    USE_SERIAL.print(getSquareBracketsStrP(str_wifi));
    USE_SERIAL.println(wifiModeInfo());
}

void setupAP(IPAddress ap_ip) {
    IPAddress gateway = ap_ip;
    IPAddress subnet = IPAddress(255, 255, 255, 0);

    WiFi.softAPConfig(ap_ip, gateway, subnet);

    USE_SERIAL.print(getSquareBracketsStrP(str_wifi));
    USE_SERIAL.print(getStrP(str_ap));
    USE_SERIAL.print(getStrP(str_arrow_dest));
    USE_SERIAL.print(ap_ip.toString());
    USE_SERIAL.print('/');
    USE_SERIAL.print(subnet.toString());
    USE_SERIAL.print(' ');
    USE_SERIAL.println(gateway.toString());
}

void setupSTA(IPAddress ip, IPAddress gateway, IPAddress subnet,
              IPAddress dns) {
    USE_SERIAL.print(getSquareBracketsStrP(str_wifi));
    USE_SERIAL.print(getStrP(str_sta));
    USE_SERIAL.print(getStrP(str_arrow_dest));
    if (ip.operator== IP4_ADDR_ANY) {
        USE_SERIAL.print(getStrP(str_dhcp));
        USE_SERIAL.print(getStrP(str_on));
    } else {
        USE_SERIAL.printf_P(strf_ip_params, ip.toString().c_str(),
                            subnet.toString().c_str(),
                            gateway.toString().c_str(), dns.toString().c_str());
    }
    USE_SERIAL.println();

    WiFi.config(ip, gateway, subnet, dns);
}

void setupSTA() {
    IPAddress any = IPAddress(IP_ADDR_ANY);
    setupSTA(any, any, any, any);
}

void startSTA(const char *ssid, const char *password) {
    USE_SERIAL.print(getSquareBracketsStrP(str_wifi));
    USE_SERIAL.print(getStrP(str_sta));
    USE_SERIAL.print(getStrP(str_arrow_dest));
    USE_SERIAL.print(getStrP(str_ssid));
    USE_SERIAL.println(getStr(ssid));

    WiFi.begin(ssid, password);

    uint8_t result = WiFi.waitForConnectResult();
    if (result != WL_CONNECTED) {
        USE_SERIAL.print(getStrP(str_wifi));
        USE_SERIAL.print(getStrP(str_sta));
        USE_SERIAL.print(getStrP(str_arrow_src));
        USE_SERIAL.println(getConnectionStatus());
    }
}

bool startAP(const char *ssid, const char *passwd, IPAddress ip) {
    bool result = false;
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
    WiFi.setAutoConnect(false);

    setMode(mode);

    staConnectedEventHandler =
        WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &e) {
            USE_SERIAL.print(getSquareBracketsStrP(str_wifi));
            USE_SERIAL.print(getStrP(str_sta));
            USE_SERIAL.print(getStrP(str_arrow_src));
            USE_SERIAL.print(getStrP(str_ssid));
            USE_SERIAL.print(getStr(e.ssid));
            USE_SERIAL.printf_P(strf_bssid, e.bssid[0], e.bssid[1], e.bssid[2],
                                e.bssid[3], e.bssid[4], e.bssid[5]);
            USE_SERIAL.printf_P(strf_channel, e.channel);
            USE_SERIAL.println();
        });

    staGotIpEventHandler =
        WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &e) {
            IPAddress ip, subnet, gateway, dns;
            ip = e.ip;
            subnet = e.mask;
            gateway = e.gw;
            dns = WiFi.dnsIP();

            USE_SERIAL.print(getSquareBracketsStrP(str_wifi));
            USE_SERIAL.print(getStrP(str_sta));
            USE_SERIAL.print(getStrP(str_arrow_src));
            USE_SERIAL.print(getStrP(str_got));
            USE_SERIAL.printf_P(strf_ip_params, ip.toString().c_str(),
                                subnet.toString().c_str(),
                                gateway.toString().c_str(),
                                dns.toString().c_str());
            USE_SERIAL.println();

            updateState();
        });

    staDisconnectedEventHandler = WiFi.onStationModeDisconnected(
        [](const WiFiEventStationModeDisconnected &e) {
            if (network == NETWORK_UP) {
                USE_SERIAL.print(getSquareBracketsStrP(str_wifi));
                USE_SERIAL.print(getStrP(str_sta));
                USE_SERIAL.print(getStrP(str_arrow_src));
                USE_SERIAL.print(getStrP(str_disconnected));
            }
            updateState();
        });

    if (mode == WLAN_STA || mode == WLAN_AP_STA) {
        const char *ssid = config->getSSID();
        const char *passwd = config->getPassword();
        const bool dhcp = config->getDHCP();
        if (dhcp) {
            setupSTA();
        } else {
            IPAddress ip, subnet, gateway, dns;
            ip = config->getIPAddr();
            subnet = config->getNetmask();
            gateway = config->getGateway();
            dns = config->getDNS();
            setupSTA(ip, gateway, subnet, dns);
        }
        startSTA(ssid, passwd);
#ifdef DEBUG_WIRELESS
        PRINTLN_WIFI_CONFIG
#endif
    }

    if (mode == WLAN_AP || mode == WLAN_AP_STA) {
        const char *ap_ssid = config->getSSID_AP();
        const char *ap_passwd = config->getPassword_AP();
        IPAddress ap_ipaddr = config->getIPAddr_AP();
#ifdef DEBUG_WIRELESS
        PRINT_WIFI_AP
        PRINT_WIFI_AP_CONFIG
#endif
        setupAP(ap_ipaddr);
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
}

void updateState() {
    switch (getWirelessMode()) {
        case WLAN_OFF:
            PRINTLN_WIFI_SWITCHED_OFF
            setNetworkState(NETWORK_DOWN);
            break;
        case WIFI_AP:
            setNetworkState(NETWORK_UP);
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

String wifiModeInfo() {
    String str = getStrP(str_mode);
    switch (Wireless::getWirelessMode()) {
        case WLAN_AP:
            str += getStrP(str_ap);
            break;
        case WLAN_STA:
            str += getStrP(str_sta);
            str += getStrP(str_reconnect);
            str += wifi_station_get_reconnect_policy() ? getStrP(str_yes)
                                                       : getStrP(str_no);
            break;
        case WLAN_AP_STA:
            str += getStrP(str_ap);
            str += getStrP(str_sta);
            break;
        case WLAN_OFF:
            str += getStrP(str_wifi);
            str += getStrP(str_off);
            break;
    }
    return str;
}

String hostIPInfo() {
    String str = getStrP(str_ip);
    switch (getWirelessMode()) {
        case WLAN_OFF:
            str += getStrP(str_none);
            break;
        case WLAN_STA:
            str += Wireless::hostSTA_IP().toString();
            break;
        case WLAN_AP:
            str += Wireless::hostAP_IP().toString();
            break;
        case WLAN_AP_STA:
            str += Wireless::hostSTA_IP().toString();
            str += " ";
            str += Wireless::hostSTA_IP().toString();
    }
    return str;
}

String RSSIInfo() {
    sint8_t rssi = wifi_station_get_rssi();
    String str = String(rssi);
    str += "dB";
    return str;
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

// https://github.com/esp8266/Arduino/issues/4114
class WiFiStationStaticIP : public ESP8266WiFiSTAClass {
   public:
    void useStaticStationIP(bool enabled) { _useStaticIp = enabled; }
};

void enableStaticStationIP(bool enabled) {
    WiFiStationStaticIP tmp;
    tmp.useStaticStationIP(enabled);
}
}  // namespace Wireless

#include "Wireless.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <user_interface.h>

#include "BuildConfig.h"
#include "ConfigHelper.h"
#include "Global.h"
#include "SysInfo.h"

namespace Wireless {

static const char str_bssid[] PROGMEM = "bssid %02x:%02x:%02x:%02x:%02x:%02x";

bool ap_enabled = false;
WirelessMode mode = WLAN_OFF;
NetworkState network = NETWORK_DOWN;
unsigned long lastNetworkUp = 0;
unsigned long lastNetworkDown = 0;

WiFiEventHandler staGotIpEventHandler, staConnectedEventHandler,
    staDisconnectedEventHandler;

NetworkStateChangeEventHandler onNetworkStateChange;

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

void changeMode(WirelessMode mode) {
    USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
    WiFi.mode((WiFiMode_t)mode);
    WiFi.disconnect();
    delay(100);
    USE_SERIAL.println(wifiModeInfo());
}

void setupAP(IPAddress ap_ip) {
    IPAddress gateway = ap_ip;
    IPAddress subnet = IPAddress(255, 255, 255, 0);

    WiFi.softAPConfig(ap_ip, gateway, subnet);

    USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
    USE_SERIAL.print(StrUtils::getStrP(str_ap));
    USE_SERIAL.print(StrUtils::getStrP(str_arrow_dest));
    USE_SERIAL.print(ap_ip.toString());
    USE_SERIAL.print('/');
    USE_SERIAL.print(subnet.toString());
    USE_SERIAL.print(' ');
    USE_SERIAL.println(gateway.toString());
}

void setupSTA(IPAddress ip, IPAddress gateway, IPAddress subnet,
              IPAddress dns) {
    USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
    USE_SERIAL.print(StrUtils::getStrP(str_sta));
    USE_SERIAL.print(StrUtils::getStrP(str_arrow_dest));
    if (ip.operator== IP4_ADDR_ANY) {
        USE_SERIAL.print(StrUtils::getStrP(str_dhcp));
        USE_SERIAL.print(StrUtils::getStrP(str_on));
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

bool startSTA(const char *ssid, const char *password) {
    USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
    USE_SERIAL.print(StrUtils::getStrP(str_sta));
    USE_SERIAL.print(StrUtils::getStrP(str_arrow_dest));
    USE_SERIAL.print(StrUtils::getStrP(str_ssid));
    USE_SERIAL.println(ssid);

    WiFi.begin(ssid, password);

    uint8_t result = WiFi.waitForConnectResult();
    if (result != WL_CONNECTED) {
        USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
        USE_SERIAL.print(StrUtils::getStrP(str_sta));
        USE_SERIAL.print(StrUtils::getStrP(str_arrow_src));
        USE_SERIAL.println(getConnectionStatus());
    }
    return result;
}

bool startAP(const char *ssid, const char *passwd) {
    return WiFi.softAP(ssid, passwd);
}

boolean disconnectWiFi() { return WiFi.disconnect(); }

void start_safe_wifi_ap() {
    USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
    USE_SERIAL.print(StrUtils::getStrP(str_safe));
    USE_SERIAL.print(StrUtils::getStrP(str_mode, false));

    char ap_ssid[32];
    strcpy(ap_ssid, APPNAME);
    strcat(ap_ssid, getChipId().c_str());
    char ap_passwd[] = "12345678";
    IPAddress ap_ipaddr = IPAddress(192, 168, 4, 1);
#ifdef DEBUG_WIRELESS
    PRINT_WIFI_AP
    PRINT_WIFI_AP_CONFIG
#endif
    changeMode(WLAN_AP);
    setupAP(ap_ipaddr);
    if (!startAP(ap_ssid, ap_passwd)) USE_SERIAL.print(StrUtils::getStrP(str_failed));
}

void init_wifi() {
    uint8_t tpw = config->getTPW();
    String host = Wireless::getConfigHostname();
    system_phy_set_max_tpw(tpw);
    WiFi.hostname(host);
    WiFi.setAutoConnect(false);

    USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
    USE_SERIAL.print(StrUtils::getStrP(str_tpw));
    USE_SERIAL.print(StrUtils::getStr(tpw));
    USE_SERIAL.print(StrUtils::getStrP(str_host));
    USE_SERIAL.println(host);
}

void start_wifi() {
    mode = (WirelessMode)config->getWiFiMode();
    init_wifi();
    changeMode(mode);

    staConnectedEventHandler =
        WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &e) {
            USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
            USE_SERIAL.print(StrUtils::getStrP(str_sta));
            USE_SERIAL.print(StrUtils::getStrP(str_arrow_src));
            USE_SERIAL.print(StrUtils::getStrP(str_ssid));
            String ssid = e.ssid;
            USE_SERIAL.print(StrUtils::getStr(ssid));
            USE_SERIAL.printf_P(str_bssid, e.bssid[0], e.bssid[1], e.bssid[2],
                                e.bssid[3], e.bssid[4], e.bssid[5]);
            USE_SERIAL.print("ch ");
            USE_SERIAL.print(e.channel);
            USE_SERIAL.println();
        });

    staGotIpEventHandler =
        WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &e) {
            IPAddress ip, subnet, gateway, dns;
            ip = e.ip;
            subnet = e.mask;
            gateway = e.gw;
            dns = WiFi.dnsIP();

            USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
            USE_SERIAL.print(StrUtils::getStrP(str_sta));
            USE_SERIAL.print(StrUtils::getStrP(str_arrow_src));
            USE_SERIAL.print(StrUtils::getStrP(str_got));
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
                USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
                USE_SERIAL.print(StrUtils::getStrP(str_sta));
                USE_SERIAL.print(StrUtils::getStrP(str_arrow_src));
                USE_SERIAL.println(StrUtils::getStrP(str_disconnected));
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
            dns = config->getDns();
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
        setupAP(ap_ipaddr);
        ap_enabled = startAP(ap_ssid, ap_passwd);
        if (ap_enabled) {
            USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
            USE_SERIAL.print(StrUtils::getStrP(str_ssid));
            USE_SERIAL.print(StrUtils::getStr(ap_ssid));
            USE_SERIAL.print(StrUtils::getStrP(str_ip));
            USE_SERIAL.println(StrUtils::getStr(ap_ipaddr));
        }
        if (ap_enabled)
            setNetworkState(NETWORK_UP);
        else
            setNetworkState(NETWORK_DOWN);
    }
}

void updateState() {
    switch (getWirelessMode()) {
        case WLAN_OFF:
            USE_SERIAL.print(StrUtils::getStrP(str_wifi));
            USE_SERIAL.print(StrUtils::getStrP(str_switched));
            USE_SERIAL.println(StrUtils::getStrP(str_off));
            setNetworkState(NETWORK_DOWN);
            break;
        case WIFI_AP:
            setNetworkState(ap_enabled ? NETWORK_UP : NETWORK_DOWN);
            break;
        case WIFI_STA:
        case WIFI_AP_STA:
            setNetworkState(WiFi.isConnected() ? NETWORK_UP : NETWORK_DOWN);
            break;
        default:
            break;
    }
}

void setNetworkState(NetworkState state) {
    if (network != state) {
        USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
        USE_SERIAL.print(StrUtils::getStrP(str_network));
        network = state;
        if (network == NETWORK_UP) {
            USE_SERIAL.print(StrUtils::getStrP(str_up));
            USE_SERIAL.print(millis_since(lastNetworkDown));
            USE_SERIAL.println(StrUtils::getStrP(str_ms, false));
            onNetworkUp();
        } else if (network == NETWORK_DOWN) {
            USE_SERIAL.print(StrUtils::getStrP(str_down));
            USE_SERIAL.print(millis_since(lastNetworkUp));
            USE_SERIAL.println(StrUtils::getStrP(str_ms, false));
            onNetworkDown();
        }
    }
}

void onNetworkUp() {
    if (onNetworkStateChange) onNetworkStateChange(true);
    lastNetworkUp = millis();
    start_services();
}

void onNetworkDown() {
    if (onNetworkStateChange) onNetworkStateChange(false);
    lastNetworkDown = millis();
}

WirelessMode getWirelessMode() { return mode = (WirelessMode)WiFi.getMode(); }

bool hasNetwork() { return network == NETWORK_UP; }

bool isActive() {
    return WiFi.softAPgetStationNum() || WiFi.status() == WL_CONNECTED;
}

void setOnNetworkStateChange(NetworkStateChangeEventHandler h) {
    onNetworkStateChange = h;
}

String networkStateInfo() {
    String str = StrUtils::getStrP(str_network);
    if (network == NETWORK_UP) {
        str += StrUtils::getStrP(str_up);
        str += getStr(millis_since(lastNetworkDown));
        onNetworkUp();
    } else if (network == NETWORK_DOWN) {
        str += StrUtils::getStrP(str_down);
        str += StrUtils::getStr(millis_since(lastNetworkUp));
        onNetworkDown();
    }
    str += StrUtils::getStrP(str_ms, false);
    return str;
}

String wifiModeInfo() {
    String str = StrUtils::getStrP(str_mode);
    switch (Wireless::getWirelessMode()) {
        case WLAN_AP:
            str += StrUtils::getStrP(str_ap);
            break;
        case WLAN_STA:
            str += StrUtils::getStrP(str_sta);
            str += StrUtils::getStrP(str_reconnect);
            str += wifi_station_get_reconnect_policy() ? StrUtils::getStrP(str_yes)
                                                       : StrUtils::getStrP(str_no);
            break;
        case WLAN_AP_STA:
            str += StrUtils::getStrP(str_ap);
            str += StrUtils::getStrP(str_sta);
            break;
        case WLAN_OFF:
            str += StrUtils::getStrP(str_switched);
            str += StrUtils::getStrP(str_off);
            break;
    }
    return str;
}

String hostIPInfo() {
    String str = StrUtils::getStrP(str_ip);
    switch (getWirelessMode()) {
        case WLAN_OFF:
            str += StrUtils::getStrP(str_none);
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
            str += Wireless::hostAP_IP().toString();
    }
    return str;
}

static const char host_name[] PROGMEM = "smartpower2";
String getConfigHostname() {
    char buf[32];
    strcpy_P(buf, host_name);
    return String(buf);
}

String getConnectionStatus() {
    station_status_t status = wifi_station_get_connect_status();
    String str;
    switch (status) {
        case STATION_CONNECTING:
            str = StrUtils::getStrP(str_connecting, false);
            break;
        case STATION_GOT_IP:
            str = StrUtils::getStrP(str_connected, false);
            break;
        case STATION_NO_AP_FOUND:
            str =
                StrUtils::getStrP(str_ap) + StrUtils::getStrP(str_not) + StrUtils::getStrP(str_found, false);
            break;
        case STATION_CONNECT_FAIL:
            str = StrUtils::getStrP(str_connection) + StrUtils::getStrP(str_failed, false);
            break;
        case STATION_WRONG_PASSWORD:
            str = StrUtils::getStrP(str_wrong) + StrUtils::getStrP(str_password, false);
            break;
        case STATION_IDLE:
            str = StrUtils::getStrP(str_idle, false);
            break;
        default:
            str = StrUtils::getStrP(str_disconnected, false);
            break;
    }
    return str;
}

void printDiag(Print *p) {
    p->println(networkStateInfo());
    p->println(wifiModeInfo());
    WiFi.printDiag(*p);
}

String RSSIInfo() {
    sint8_t rssi = wifi_station_get_rssi();
    String str = String(rssi);
    str += " dB";
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

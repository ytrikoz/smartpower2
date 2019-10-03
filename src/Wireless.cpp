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
Mode wlan = WLAN_OFF;
NetworkStatus networkStatus = NETWORK_DOWN;
unsigned long lastNetworkUp = 0;
unsigned long lastNetworkDown = 0;

WiFiEventHandler staGotIpEventHandler, staConnectedEventHandler,
    staDisconnectedEventHandler;

NetworkStatusChangeEventHandler statusChangeHandler;

IPAddress hostAP_IP() { return WiFi.softAPIP(); }

IPAddress hostSTA_IP() { return WiFi.localIP(); }

String hostAP_Password() { return WiFi.softAPPSK(); }

String hostAP_SSID() { return WiFi.softAPSSID(); }

String hostSTA_SSID() { return WiFi.SSID(); }

String hostSSID() {
    String str;
    switch (getMode()) {
    case WLAN_STA:
    case WLAN_AP_STA:
        str = hostSTA_SSID();
        break;
    case WLAN_AP:
        str = hostAP_SSID();
        break;
    case WLAN_OFF:
        str = "";
        break;
    default:
        break;
    }
    return str;
}

String hostName() {
    String str;
    switch (getMode()) {
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

IPAddress hostIP() {
    IPAddress ip;
    switch (getMode()) {
    case WLAN_OFF:
        break;
    case WLAN_STA:
    case WLAN_AP_STA:
        ip = hostSTA_IP();
        break;
    case WLAN_AP:
        ip = hostAP_IP();
        break;
    }
    return ip;
}

void changeMode(Mode wlan) {
    USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
    WiFi.mode((WiFiMode_t)wlan);
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
    strcpy(ap_ssid, APP_NAME);
    strcat(ap_ssid, getChipId().c_str());
    char ap_passwd[] = "12345678";
    IPAddress ap_ipaddr = IPAddress(192, 168, 4, 1);
#ifdef DEBUG_WIRELESS
    PRINT_WIFI_AP
    PRINT_WIFI_AP_CONFIG
#endif
    changeMode(WLAN_AP);
    setupAP(ap_ipaddr);
    if (!startAP(ap_ssid, ap_passwd))
        USE_SERIAL.print(StrUtils::getStrP(str_failed));
}

void init_wifi() {
    uint8_t tpw = app.getConfig()->getValueAsByte(TPW);
    String host = getConfigHostname();
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
    wlan = (Mode)app.getConfig()->getValueAsByte(WIFI);
    init_wifi();
    changeMode(wlan);

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
            if (networkStatus == NETWORK_UP) {
                USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
                USE_SERIAL.print(StrUtils::getStrP(str_sta));
                USE_SERIAL.print(StrUtils::getStrP(str_arrow_src));
                USE_SERIAL.println(StrUtils::getStrP(str_disconnected));
            }
            updateState();
        });

    if (wlan == WLAN_STA || wlan == WLAN_AP_STA) {
        const char *ssid = app.getConfig()->getValueAsString(SSID);
        const char *passwd = app.getConfig()->getValueAsString(PASSWORD);
        const bool dhcp = app.getConfig()->getValueAsBool(DHCP);
        if (dhcp) {
            setupSTA();
        } else {
            IPAddress ip, subnet, gateway, dns;
            ip = app.getConfig()->getValueAsIPAddress(IPADDR);
            subnet = app.getConfig()->getValueAsIPAddress(NETMASK);
            gateway = app.getConfig()->getValueAsIPAddress(GATEWAY);
            dns = app.getConfig()->getValueAsIPAddress(DNS);
            setupSTA(ip, gateway, subnet, dns);
        }
        if (startSTA(ssid, passwd)) {
            if (wlan == WLAN_AP_STA)
                setBroadcastTo(3);
        }
#ifdef DEBUG_WIRELESS
        PRINTLN_WIFI_CONFIG
#endif
    }

    if (wlan == WLAN_AP || wlan == WLAN_AP_STA) {
        const char *ap_ssid = app.getConfig()->getValueAsString(AP_SSID);
        const char *ap_passwd = app.getConfig()->getValueAsString(AP_PASSWORD);
        IPAddress ap_ipaddr = app.getConfig()->getValueAsIPAddress(AP_IPADDR);
        setupAP(ap_ipaddr);
        ap_enabled = startAP(ap_ssid, ap_passwd);
        if (ap_enabled) {
            USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
            USE_SERIAL.print(StrUtils::getStrP(str_ssid));
            USE_SERIAL.print(ap_ssid);
            USE_SERIAL.print(' ');
            USE_SERIAL.print(StrUtils::getStrP(str_ip));
            USE_SERIAL.println(StrUtils::getStr(ap_ipaddr));
        }
        if (ap_enabled)
            setNetworkStatus(NETWORK_UP);
        else
            setNetworkStatus(NETWORK_DOWN);
    }
}

void setBroadcastTo(uint8_t _if) {
    String old_if = String(wifi_get_broadcast_if(), DEC);
    USE_SERIAL.print(StrUtils::getIdentStrP(str_wifi));
    USE_SERIAL.print(StrUtils::getStrP(str_set));
    USE_SERIAL.print(StrUtils::getStrP(str_broadcast));
    USE_SERIAL.print(StrUtils::getStr(old_if));
    USE_SERIAL.print(StrUtils::getStrP(str_arrow_dest));
    USE_SERIAL.print(_if);
    if (!wifi_set_broadcast_if(_if))
        USE_SERIAL.print(StrUtils::getStrP(str_failed));
    USE_SERIAL.println();
}

void updateState() {
    switch (getMode()) {
    case WLAN_OFF:
        USE_SERIAL.print(StrUtils::getStrP(str_wifi));
        USE_SERIAL.print(StrUtils::getStrP(str_switched));
        USE_SERIAL.println(StrUtils::getStrP(str_off));
        setNetworkStatus(NETWORK_DOWN);
        break;
    case WIFI_AP:
        setNetworkStatus(ap_enabled ? NETWORK_UP : NETWORK_DOWN);
        break;
    case WIFI_STA:
    case WIFI_AP_STA:
        setNetworkStatus(WiFi.isConnected() ? NETWORK_UP : NETWORK_DOWN);
        break;
    default:
        break;
    }
}

void setNetworkStatus(NetworkStatus status) {
    if (networkStatus != status) {
        networkStatus = status;
        if (statusChangeHandler)
            statusChangeHandler(hasNetwork(), millis());
    }
}

void onNetworkUp() { lastNetworkUp = millis(); }

void onNetworkDown() { lastNetworkDown = millis(); }

Mode getMode() { return wlan = (Mode)WiFi.getMode(); }

bool hasNetwork() { return networkStatus == NETWORK_UP; }

bool isActive() {
    return WiFi.softAPgetStationNum() || WiFi.status() == WL_CONNECTED;
}

void setOnNetworkStatusChange(NetworkStatusChangeEventHandler h) {
    statusChangeHandler = h;

    if (statusChangeHandler)
        statusChangeHandler(hasNetwork(), millis());
}

String networkStateInfo() {
    char buf[8];
    String str = StrUtils::getStrP(str_network);
    str += StrUtils::getUpDownStr(buf, hasNetwork());
    str += StrUtils::getStr(
        millis_since(hasNetwork() ? lastNetworkDown : lastNetworkUp));
    return str;
}

String wifiModeInfo() {
    String str = StrUtils::getStrP(str_mode);
    switch (Wireless::getMode()) {
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
    switch (getMode()) {
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
        str = StrUtils::getStrP(str_ap) + StrUtils::getStrP(str_not) +
              StrUtils::getStrP(str_found, false);
        break;
    case STATION_CONNECT_FAIL:
        str = StrUtils::getStrP(str_connection) +
              StrUtils::getStrP(str_failed, false);
        break;
    case STATION_WRONG_PASSWORD:
        str = StrUtils::getStrP(str_wrong) +
              StrUtils::getStrP(str_password, false);
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

size_t printDiag(Print *p) {
    size_t n = p->println(networkStateInfo());
    n += p->println(wifiModeInfo());
    WiFi.printDiag(*p);
    return n;
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
} // namespace Wireless

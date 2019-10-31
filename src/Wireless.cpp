#include "Wireless.h"

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <user_interface.h>

#include "BuildConfig.h"
#include "ConfigHelper.h"
#include "Global.h"
#include "SysInfo.h"

using namespace PrintUtils;
using namespace StrUtils;

namespace Wireless {

static const char strf_bssid[] PROGMEM = "%02x:%02x:%02x:%02x:%02x:%02x";
static const char strf_ip_params[] PROGMEM =
    "ip: %s subnet: %s gateway: %s dns: %s";

bool ap_enabled = false;
Mode mode = WLAN_OFF;
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

void changeMode(Mode wlan_mode) {
    print_ident(&DEBUG, FPSTR(str_wifi));
    WiFi.mode((WiFiMode_t)wlan_mode);
    delay(100);
    print_wifi_mode(&DEBUG);
    print_ln(&DEBUG);
}

void setupAP(IPAddress ipaddr) {
    IPAddress gateway = ipaddr;
    IPAddress subnet = IPAddress(255, 255, 255, 0);

    WiFi.softAPConfig(ipaddr, gateway, subnet);

    IPAddress dns = WiFi.dnsIP();

    char buf[64];
    sprintf_P(buf, strf_ip_params, ipaddr.toString().c_str(),
              subnet.toString().c_str(), gateway.toString().c_str(),
              dns.toString().c_str());
    print_ident(&DEBUG, FPSTR(str_wifi));
    println(&DEBUG, buf);
}

void setupSTA() {
    IPAddress any = IPAddress(IP_ADDR_ANY);
    setupSTA(any, any, any, any);
}

void setupSTA(IPAddress ipaddr, IPAddress subnet, IPAddress gateway,
              IPAddress dns) {
    print_ident(&DEBUG, FPSTR(str_wifi));
    if (ipaddr == IPAddress(IP4_ADDR_ANY)) {
        println_nameP_value(&DEBUG, str_dhcp, FPSTR(str_on));
    } else {
        char buf[64];
        sprintf_P(buf, strf_ip_params, ipaddr.toString().c_str(),
                  subnet.toString().c_str(), gateway.toString().c_str(),
                  dns.toString().c_str());
        print_ident(&DEBUG, FPSTR(str_wifi));
        println(&DEBUG, buf);
    }
    WiFi.config(ipaddr, gateway, subnet, dns);
}

bool startSTA(const char *ssid, const char *password) {
    print_ident(&DEBUG, FPSTR(str_wifi));
    println_nameP_value(&DEBUG, str_ssid, ssid);

    WiFi.begin(ssid, password);

    uint8_t result = WiFi.waitForConnectResult();
    if (result != WL_CONNECTED) {
        print_ident(&DEBUG, FPSTR(str_wifi));
        println_nameP_value(&DEBUG, str_status, getConnectionStatus().c_str());
    }
    return result;
}

bool startAP(const char *ssid, const char *passwd) {
    return WiFi.softAP(ssid, passwd);
}

boolean disconnectWiFi() { return WiFi.disconnect(); }

void start_safe_wifi_ap() {
    print_ident(&DEBUG, FPSTR(str_wifi));
    print(&DEBUG, FPSTR(str_safe));
    print(&DEBUG, FPSTR(str_mode));

    char ap_ssid[32];
    strcpy(ap_ssid, APP_NAME);
    strcat(ap_ssid, getChipId().c_str());
    char ap_passwd[] = "12345678";
    IPAddress ap_ipaddr = IPAddress(192, 168, 4, 1);

    changeMode(WLAN_AP);
    setupAP(ap_ipaddr);
    if (!startAP(ap_ssid, ap_passwd))
        print(&DEBUG, FPSTR(str_failed));
    print_ln(&DEBUG);
}

void init() {
    uint8_t tpw = app.getConfig()->getValueAsByte(TPW);
    String host = getConfigHostname();
    system_phy_set_max_tpw(tpw);
    WiFi.hostname(host);
    WiFi.setAutoConnect(false);

    mode = (Mode)app.getConfig()->getValueAsByte(WIFI);
    changeMode(mode);

    print_ident(&DEBUG, FPSTR(str_wifi));
    println_nameP_value(&DEBUG, str_tpw, tpw);

    print_ident(&DEBUG, FPSTR(str_wifi));
    println_nameP_value(&DEBUG, str_host, host);
}

void start_wifi() {
    init();

    staConnectedEventHandler =
        WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &e) {
            char buf[32];
            sprintf_P(buf, strf_bssid, e.bssid[0], e.bssid[1], e.bssid[2],
                      e.bssid[3], e.bssid[4], e.bssid[5]);

            print_ident(&DEBUG, FPSTR(str_wifi));
            println_nameP_value(&DEBUG, str_bssid, buf);

            print_ident(&DEBUG, FPSTR(str_wifi));
            println_nameP_value(&DEBUG, str_ch, e.channel);
        });

    staGotIpEventHandler =
        WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &e) {
            IPAddress ip, subnet, gateway, dns;
            ip = e.ip;
            subnet = e.mask;
            gateway = e.gw;
            dns = WiFi.dnsIP();

            print_ident(&DEBUG, FPSTR(str_wifi));
            print(&DEBUG, FPSTR(str_got));
            char buf[128];
            sprintf_P(buf, strf_ip_params, ip.toString().c_str(),
                      subnet.toString().c_str(), gateway.toString().c_str(),
                      dns.toString().c_str());
            println(&DEBUG, buf);
            updateState();
        });

    staDisconnectedEventHandler = WiFi.onStationModeDisconnected(
        [](const WiFiEventStationModeDisconnected &e) {
            if (networkStatus == NETWORK_UP) {
                print_ident(&DEBUG, FPSTR(str_wifi));
                println(&DEBUG, FPSTR(str_disconnected));
            }
            updateState();
        });

    if (mode == WLAN_STA || mode == WLAN_AP_STA) {
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
            if (mode == WLAN_AP_STA)
                setBroadcastTo(3);
        }
    }

    if (mode == WLAN_AP || mode == WLAN_AP_STA) {
        const char *ap_ssid = app.getConfig()->getValueAsString(AP_SSID);
        const char *ap_passwd = app.getConfig()->getValueAsString(AP_PASSWORD);
        IPAddress ap_ipaddr = app.getConfig()->getValueAsIPAddress(AP_IPADDR);
        setupAP(ap_ipaddr);

        print_ident(&DEBUG, FPSTR(str_wifi));
        if ((ap_enabled = startAP(ap_ssid, ap_passwd))) {
            println_nameP_value(&DEBUG, str_ssid, WiFi.SSID().c_str());
        } else {
            println(&DEBUG, FPSTR(str_failed));
        }

        setNetworkStatus(ap_enabled ? NETWORK_UP : NETWORK_DOWN);
    }
}

void setBroadcastTo(uint8_t _if) {
    String old_if = String(wifi_get_broadcast_if(), DEC);
    DEBUG.print(StrUtils::getIdentStrP(str_wifi));
    DEBUG.print(StrUtils::getStrP(str_set));
    DEBUG.print(StrUtils::getStrP(str_broadcast));
    DEBUG.print(StrUtils::getStr(old_if));
    DEBUG.print(StrUtils::getStrP(str_arrow_dest));
    DEBUG.print(_if);
    if (!wifi_set_broadcast_if(_if))
        DEBUG.print(StrUtils::getStrP(str_failed));
    DEBUG.println();
}

void updateState() {
    switch (getMode()) {
    case WLAN_OFF:
        DEBUG.print(StrUtils::getStrP(str_wifi));
        DEBUG.print(StrUtils::getStrP(str_switched_off));
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

Mode getMode() { return mode = (Mode)WiFi.getMode(); }

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

void print_wifi_mode(Print *p) {
    p->print(FPSTR(str_mode));
    p->print(':');
    p->print(' ');
    switch (Wireless::getMode()) {
    case WLAN_AP:
        p->print(FPSTR(str_ap));
        break;
    case WLAN_STA:
        p->print(FPSTR(str_sta));
        break;
    case WLAN_AP_STA:
        p->print(FPSTR(str_ap_sta));
        break;
    case WLAN_OFF:
        p->print(FPSTR(str_switched_off));
        return;
    }
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
    size_t n = println_nameP_value(p, str_network,
                                   StrUtils::getUpDownStr(hasNetwork()));
    n += println_nameP_value(
        p, str_for,
        millis_since(hasNetwork() ? lastNetworkDown : lastNetworkUp));
    n += println_nameP_value(p, str_mode, getMode());
    WiFi.printDiag(*p);
    return n;
}

String RSSIInfo() {
    sint8_t rssi = wifi_station_get_rssi();
    char buf[16];
    sprintf(buf, "%d dB", rssi);
    String str(buf);
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

int hex2num(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return -1;
}

int hex2byte(const char *hex) {
    int a, b;
    a = hex2num(*hex++);
    if (a < 0)
        return -1;
    b = hex2num(*hex++);
    if (b < 0)
        return -1;
    return (a << 4) | b;
}

bool atomac(const char *txt, uint8_t *addr) {
    for (uint8_t i = 0; i < 6; i++) {
        int a, b;
        a = hex2num(*txt++);
        if (a < 0)
            return false;
        b = hex2num(*txt++);
        if (b < 0)
            return false;
        *addr++ = (a << 4) | b;
        if (i < 5 && *txt++ != ':')
            return false;
    }
    return true;
}

} // namespace Wireless

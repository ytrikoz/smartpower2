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

bool ap_enabled = false;

NetworkMode networkMode = NETWORK_OFF;

NetworkStatus networkStatus = NETWORK_DOWN;

unsigned long lastUp, lastDown = 0;

WiFiEventHandler staGotIpEventHandler, staConnectedEventHandler,
    staDisconnectedEventHandler;

NetworkStatusChangeEventHandler statusChangeHandler;

String hostAP_SSID() { return WiFi.softAPSSID(); }

String hostAP_Password() { return WiFi.softAPPSK(); }

IPAddress hostAP_IP() { return WiFi.softAPIP(); }

String hostSTA_SSID() { return WiFi.SSID(); }

IPAddress hostSTA_IP() { return WiFi.localIP(); }

IPAddress hostDNS() {
    IPAddress res(IPADDR_NONE);
    switch (getMode()) {
    case NETWORK_OFF:
        break;
    case NETWORK_AP:
        res = WiFi.softAPIP();
        break;
    case NETWORK_STA:
    case NETWORK_AP_STA:
        res = WiFi.dnsIP();
        break;
    }
    return res;
}

String hostSSID() {
    String res;
    switch (getMode()) {
    case NETWORK_OFF:
        break;
    case NETWORK_AP:
        res = hostAP_SSID();
        break;
    case NETWORK_STA:
    case NETWORK_AP_STA:
        res = hostSTA_SSID();
        break;
    }
    return res;
}

String hostMac() {
    String str;
    switch (getMode()) {
    case NETWORK_OFF:
        break;
    case NETWORK_AP:
        str = WiFi.softAPmacAddress();
        break;
    case NETWORK_STA:
    case NETWORK_AP_STA:
        str = WiFi.macAddress();
        break;
    }
    return str;
}

String hostName() {
    String str;
    switch (getMode()) {
    case NETWORK_STA:
    case NETWORK_AP_STA:
        str += WiFi.hostname();
        break;
    case NETWORK_OFF:
    case NETWORK_AP:
        str += hostAP_Name();
        break;
    }
    return str;
}

IPAddress hostSubnet() {
    IPAddress res(IPADDR_NONE);
    switch (getMode()) {
    case NETWORK_OFF:
        break;
    case NETWORK_AP:
        res = IPAddress(255, 255, 255, 0);
        break;
    case NETWORK_STA:
    case NETWORK_AP_STA:
        res = WiFi.subnetMask();
        break;
    default:
        break;
    }
    return res;
}

IPAddress hostGateway() {
    IPAddress res(IPADDR_NONE);
    switch (getMode()) {
    case NETWORK_OFF:
        break;
    case NETWORK_AP:
        res = WiFi.softAPIP();
        break;
    case NETWORK_STA:
    case NETWORK_AP_STA:
        res = WiFi.gatewayIP();
        break;
    default:
        break;
    }
    return res;
}

IPAddress hostIP() {
    IPAddress res(IPADDR_NONE);
    switch (getMode()) {
    case NETWORK_OFF:
        break;
    case NETWORK_AP:
        res = hostAP_IP();
        break;
    case NETWORK_STA:
    case NETWORK_AP_STA:
        res = hostSTA_IP();
        break;
    }
    return res;
}

void setMode(const NetworkMode mode) {
    print_ident(&DEBUG, FPSTR(str_wifi));
    println_nameP_value(&DEBUG, str_mode, getModeStr(mode));
    WiFi.mode((WiFiMode_t)mode);
    delay(100);
}

void setupAP(const IPAddress host) {
    print_ident(&DEBUG, FPSTR(str_wifi));
    print_ident(&DEBUG, FPSTR(str_ap));

    const IPAddress gateway(host);
    const IPAddress subnet(255, 255, 255, 0);
    WiFi.softAPConfig(host, gateway, subnet);

    println(&DEBUG, formatNetwork(host, subnet, gateway));
}

void setupSTA() {
    const IPAddress any(IP_ADDR_ANY);
    setupSTA(any, any, any, any);
}

void setupSTA(const IPAddress ipaddr, const IPAddress subnet,
              const IPAddress gateway, const IPAddress dns) {
    print_ident(&DEBUG, FPSTR(str_wifi));
    print_ident(&DEBUG, FPSTR(str_sta));

    if (ipaddr == IPAddress(IP4_ADDR_ANY)) {
        println_nameP_value(&DEBUG, str_dhcp, getBoolStr(true).c_str());
    } else {
        char buf[128];
        sprintf(buf, "ip: %s subnet: %s gateway: %s dns: %s",
                ipaddr.toString().c_str(), subnet.toString().c_str(),
                gateway.toString().c_str(), dns.toString().c_str());
        println(&DEBUG, buf);
    }
    WiFi.config(ipaddr, gateway, subnet, dns);
}

bool startSTA(const char *ssid, const char *passwd) {
    WiFi.begin(ssid, passwd);
    uint8_t result = WiFi.waitForConnectResult();
    print_ident(&DEBUG, FPSTR(str_wifi));
    print_ident(&DEBUG, FPSTR(str_sta));
    print_nameP_value(&DEBUG, str_ssid, ssid);
    println_nameP_value(&DEBUG, str_status, hostSTA_StatusStr());
    return result;
}

bool startAP(const char *ssid, const char *passwd) {
    bool res = WiFi.softAP(ssid, passwd);
    print_ident(&DEBUG, FPSTR(str_wifi));
    print_ident(&DEBUG, FPSTR(str_ap));
    if (res) {
        println_nameP_value(&DEBUG, str_ssid, WiFi.softAPSSID().c_str());
    } else {
        println(&DEBUG, FPSTR(str_failed));
    }
    return res;
}

boolean disconnectWiFi() { return WiFi.disconnect(); }

void start_safe_wifi_ap() {
    print_ident(&DEBUG, FPSTR(str_wifi));
    print_ident(&DEBUG, FPSTR(str_ap));
    print(&DEBUG, FPSTR(str_safe));
    print(&DEBUG, FPSTR(str_mode));

    char ap_ssid[32];
    strcpy(ap_ssid, APP_NAME);
    strcat(ap_ssid, SysInfo::getChipId().c_str());
    IPAddress ap_ipaddr = IPAddress(192, 168, 4, 1);
    char ap_passwd[] = "12345678";
    setMode(NETWORK_AP);
    setupAP(ap_ipaddr);
    if (!startAP(ap_ssid, ap_passwd))
        print(&DEBUG, FPSTR(str_failed));
    print_ln(&DEBUG);
}

void start() {
    String host = hostAP_Name();
    print_ident(&DEBUG, FPSTR(str_wifi));
    println_nameP_value(&DEBUG, str_host, host);

    uint8_t tpw = app.getConfig()->getValueAsByte(TPW);
    print_ident(&DEBUG, FPSTR(str_wifi));
    println_nameP_value(&DEBUG, str_tpw, tpw);

    NetworkMode mode = (NetworkMode)app.getConfig()->getValueAsByte(WIFI);
    init(mode, host.c_str(), tpw);

    staConnectedEventHandler =
        WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &e) {
            char buf[32];
            sprintf(buf, MACSTR, MAC2STR(e.bssid));
            print_ident(&DEBUG, FPSTR(str_wifi));
            print_ident(&DEBUG, FPSTR(str_sta));
            print_nameP_value(&DEBUG, str_ch, e.channel);
            println_nameP_value(&DEBUG, str_bssid, buf);
        });

    staGotIpEventHandler =
        WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &e) {
            print_ident(&DEBUG, FPSTR(str_wifi));
            print_ident(&DEBUG, FPSTR(str_sta));
            print(&DEBUG, FPSTR(str_got));
            char buf[128];
            sprintf(buf, "ip: %s subnet: %s gateway: %s",
                    e.ip.toString().c_str(), e.mask.toString().c_str(),
                    e.gw.toString().c_str());
            println(&DEBUG, buf);
            updateState();
        });

    staDisconnectedEventHandler = WiFi.onStationModeDisconnected(
        [](const WiFiEventStationModeDisconnected &e) {
            if (networkStatus == NETWORK_UP) {
                print_ident(&DEBUG, FPSTR(str_wifi));
                print_ident(&DEBUG, FPSTR(str_sta));
                println(&DEBUG, FPSTR(str_disconnected));
            }
            updateState();
        });

    if (mode == NETWORK_STA || mode == NETWORK_AP_STA) {
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
            if (mode == NETWORK_AP_STA)
                setBroadcast(3);
        }
    }

    if (mode == NETWORK_AP || mode == NETWORK_AP_STA) {
        const char *ap_ssid = app.getConfig()->getValueAsString(AP_SSID);
        const char *ap_passwd = app.getConfig()->getValueAsString(AP_PASSWORD);
        IPAddress ap_ipaddr = app.getConfig()->getValueAsIPAddress(AP_IPADDR);

        setupAP(ap_ipaddr);

        ap_enabled = startAP(ap_ssid, ap_passwd);

        setNetworkStatus(ap_enabled ? NETWORK_UP : NETWORK_DOWN);
    }
}

void init(NetworkMode mode, const char *host, uint8_t tpw) {
    system_phy_set_max_tpw(tpw);
    WiFi.hostname(host);
    WiFi.setAutoConnect(false);
    setMode(mode);
}

void setBroadcast(uint8_t _new) {
    print_ident(&DEBUG, FPSTR(str_wifi));
    uint8_t _old = wifi_get_broadcast_if();
    print(&DEBUG, FPSTR(str_broadcast));
    print(&DEBUG, _old);
    print(&DEBUG, FPSTR(str_arrow_dest));
    print(&DEBUG, _new);
    if (!wifi_set_broadcast_if(_new))
        print(&DEBUG, FPSTR(str_failed));
    print_ln(&DEBUG);
}

void updateState() {
    NetworkMode mode = getMode();
    bool res = false;
    switch (mode) {
    case NETWORK_OFF:
        break;
    case WIFI_AP:
        res = ap_enabled;
        break;
    case WIFI_STA:
    case WIFI_AP_STA:
        res = WiFi.isConnected();
        break;
    default:
        break;
    }
    setNetworkStatus(res ? NETWORK_UP : NETWORK_DOWN);
}

void setNetworkStatus(NetworkStatus status) {
    if (networkStatus != status) {
        networkStatus = status;
        if (statusChangeHandler)
            statusChangeHandler(status, millis());
    }
}

void onNetworkUp() { lastUp = millis(); }

void onNetworkDown() { lastDown = millis(); }

NetworkMode getMode() {
    networkMode = (NetworkMode)WiFi.getMode();
    return networkMode;
}

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
    bool has = hasNetwork();
    String str = FPSTR(str_network);
    str += getUpDownStr(has);
    str += millis_since(has ? lastDown : lastUp);
    return str;
}

String getModeStr(NetworkMode mode) {
    PGM_P strP = str_unknown;
    switch (mode) {
    case NETWORK_OFF:
        strP = str_off;
        break;
    case NETWORK_AP:
        strP = str_ap;
        break;
    case NETWORK_STA:
        strP = str_sta;
        break;
    case NETWORK_AP_STA:
        strP = str_ap_sta;
        break;
    }
    return FPSTR(strP);
}

String hostIPInfo() {
    String str = getStrP(str_ip);
    switch (getMode()) {
    case NETWORK_OFF:
        str += getStrP(str_none);
        break;
    case NETWORK_STA:
        str += hostSTA_IP().toString();
        break;
    case NETWORK_AP:
        str += hostAP_IP().toString();
        break;
    case NETWORK_AP_STA:
        str += hostSTA_IP().toString();
        str += " ";
        str += hostAP_IP().toString();
    }
    return str;
}

String hostAP_Name() {
    char host_name[] = "smartpower2";
    char buf[16];
    strcpy(buf, host_name);
    return String(buf);
}

String hostSTA_StatusStr() {
    PGM_P strP = str_unknown;
    switch (wifi_station_get_connect_status()) {
    case STATION_CONNECTING:
        strP = str_connecting;
        break;
    case STATION_GOT_IP:
        strP = str_connected;
        break;
    case STATION_NO_AP_FOUND:
        strP = str_ap_not_found;
        break;
    case STATION_CONNECT_FAIL:
        strP = str_connection_failed;
        break;
    case STATION_WRONG_PASSWORD:
        strP = str_wrong_password;
        break;
    case STATION_IDLE:
        strP = str_idle;
        break;
    }
    return FPSTR(strP);
}

size_t printDiag(Print *p) {
    size_t n = println_nameP_value(p, str_network, getUpDownStr(hasNetwork()));
    n += println_nameP_value(p, str_for,
                             millis_since(hasNetwork() ? lastDown : lastUp));
    n += println_nameP_value(p, str_mode, getMode());
    WiFi.printDiag(*p);
    return n;
}

String getRSSI() {
    char buf[32];
    sprintf(buf, "%d dB", WiFi.RSSI());
    return String(buf);
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

String getWifiChannel() { return String(WiFi.channel()); }

String getWiFiPhyMode() {
    char buf[8];
    strcpy(buf, "801.11");
    switch (WiFi.getPhyMode()) {
    case WIFI_PHY_MODE_11B:
        buf[6] = 'b';
        break;
    case WIFI_PHY_MODE_11G:
        buf[6] = 'g';
        break;
    case WIFI_PHY_MODE_11N:
        buf[6] = 'n';
        break;
    }
    buf[7] = '\x00';
    return String(buf);
}

String getUniqueName() {
    char buf[16];
    strcpy(buf, APP_SHORT "_");
    strcat(buf, SysInfo::getChipId().c_str());
    return String(buf);
}

} // namespace Wireless

// https://github.com/esp8266/Arduino/issues/4114
class WiFiStationStaticIP : public ESP8266WiFiSTAClass {
  public:
    void useStaticStationIP(bool enabled) { _useStaticIp = enabled; }
};

void enableStaticStationIP(bool enabled) {
    WiFiStationStaticIP tmp;
    tmp.useStaticStationIP(enabled);
}

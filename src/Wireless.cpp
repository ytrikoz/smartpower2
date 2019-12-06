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

WiFiEventHandler stationConnectedHandler, stationDisconnectedHandler,
    probeRequestHandler;

NetworkStatusChangeEventHandler statusChangeHandler;

String hostAP_SSID() { return WiFi.softAPSSID(); }

String hostAP_Password() { return WiFi.softAPPSK(); }

IPAddress hostAP_IP() { return WiFi.softAPIP(); }

String hostSTA_RSSI() { return fmt_rssi(WiFi.RSSI()); }

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

const String hostName() {
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
    print_wifi_ap(&DEBUG);

    const IPAddress gateway(host);
    const IPAddress subnet(255, 255, 255, 0);

    WiFi.softAPConfig(host, gateway, subnet);

    println(&DEBUG, fmt_network(host, subnet, gateway));
}

void setupSTA() {
    const IPAddress any(IP_ADDR_ANY);
    setupSTA(any, any, any, any);
}

void setupSTA(const IPAddress ipaddr, const IPAddress subnet,
              const IPAddress gateway, const IPAddress dns) {
    print_wifi_sta(&DEBUG);
    if (ipaddr == IPAddress(IP4_ADDR_ANY)) {
        println_nameP_value(&DEBUG, str_dhcp, getBoolStr(true).c_str());
    } else {
        println(&DEBUG, fmt_network(ipaddr, subnet, gateway, dns));
    }
    WiFi.config(ipaddr, gateway, subnet, dns);
}

bool startSTA(const char *ssid, const char *passwd) {
    WiFi.begin(ssid, passwd);
    uint8_t result = WiFi.waitForConnectResult();
    print_wifi_sta(&DEBUG);
    print_nameP_value(&DEBUG, str_ssid, ssid);
    println_nameP_value(&DEBUG, str_status, hostSTA_StatusStr());
    return result;
}

bool startAP(const char *ssid, const char *passwd) {
    bool res = WiFi.softAP(ssid, passwd);
    print_wifi_ap(&DEBUG);
    if (res) {
        println_nameP_value(&DEBUG, str_ssid, WiFi.softAPSSID().c_str());
    } else {
        println(&DEBUG, FPSTR(str_failed));
    }
    return res;
}

boolean disconnectWiFi() { return WiFi.disconnect(); }

void start_safe_wifi_ap() {
    String ap_ssid = SysInfo::getUniqueName();
    print(&DEBUG, getIdentStrP(str_wifi), FPSTR(str_safe), FPSTR(str_ssid), ap_ssid);

    IPAddress ap_ipaddr = IPAddress(192, 168, 4, 1);
    char ap_passwd[] = "12345678";
    setMode(NETWORK_AP);
    setupAP(ap_ipaddr);
    startAP(ap_ssid.c_str(), ap_passwd);
}

void start() {
    String host = hostAP_Name();
    print_ident(&DEBUG, FPSTR(str_wifi));
    println_nameP_value(&DEBUG, str_host, host.c_str());

    uint8_t tpw = app.params()->getValueAsByte(TPW);
    print_ident(&DEBUG, FPSTR(str_wifi));
    println_nameP_value(&DEBUG, str_tpw, tpw);

    NetworkMode mode = (NetworkMode)app.params()->getValueAsByte(WIFI);
    init(mode, host.c_str(), tpw);

    stationConnectedHandler = WiFi.onSoftAPModeStationConnected(
        [](const WiFiEventSoftAPModeStationConnected &e) {
            print_wifi_ap(&DEBUG);
            print(&DEBUG, FPSTR(str_connected));
            print_wifi_ap_station(&DEBUG, e.aid, e.mac);
        });

    stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(
        [](const WiFiEventSoftAPModeStationDisconnected &e) {
            print_wifi_ap(&DEBUG);
            print(&DEBUG, FPSTR(str_disconnected));
            print_wifi_ap_station(&DEBUG, e.aid, e.mac);
        });

    /*
    probeRequestHandler = WiFi.onSoftAPModeProbeRequestReceived(
        [](const WiFiEventSoftAPModeProbeRequestReceived &e) {
            print_wifi_ap(&DEBUG);
            print(&DEBUG, FPSTR(str_probe));
            print_paramP_value(&DEBUG, str_mac, fmt_mac(e.mac).c_str());
            print_paramP_value(&DEBUG, str_rssi, fmt_rssi(e.rssi).c_str());
        });
*/
    staConnectedEventHandler =
        WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &e) {
            print_wifi_sta(&DEBUG);
            print_nameP_value(&DEBUG, str_bssid, fmt_mac(e.bssid).c_str());
            println_nameP_value(&DEBUG, str_ch, e.channel);
        });

    staGotIpEventHandler =
        WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &e) {
            print_wifi_sta(&DEBUG);
            print(&DEBUG, FPSTR(str_got));
            println(&DEBUG, fmt_network(e.ip, e.mask, e.gw).c_str());
            updateState();
        });

    staDisconnectedEventHandler = WiFi.onStationModeDisconnected(
        [](const WiFiEventStationModeDisconnected &e) {
            if (networkStatus == NETWORK_UP) {
                print_wifi_sta(&DEBUG);
                println(&DEBUG, FPSTR(str_disconnected));
            }
            updateState();
        });

    if (mode == NETWORK_STA || mode == NETWORK_AP_STA) {
        const char *ssid = app.params()->getValueAsString(SSID);
        const char *passwd = app.params()->getValueAsString(PASSWORD);
        const bool dhcp = app.params()->getValueAsBool(DHCP);

        if (dhcp) {
            setupSTA();
        } else {
            IPAddress ip, subnet, gateway, dns;
            ip = app.params()->getValueAsIPAddress(IPADDR);
            subnet = app.params()->getValueAsIPAddress(NETMASK);
            gateway = app.params()->getValueAsIPAddress(GATEWAY);
            dns = app.params()->getValueAsIPAddress(DNS);
            setupSTA(ip, gateway, subnet, dns);
        }

        if (startSTA(ssid, passwd)) {
            if (mode == NETWORK_AP_STA)
                setBroadcast(3);
        }
    }

    if (mode == NETWORK_AP || mode == NETWORK_AP_STA) {
        const char *ap_ssid = app.params()->getValueAsString(AP_SSID);
        const char *ap_passwd = app.params()->getValueAsString(AP_PASSWORD);
        IPAddress ap_ipaddr = app.params()->getValueAsIPAddress(AP_IPADDR);

        setupAP(ap_ipaddr);

        ap_enabled = startAP(ap_ssid, ap_passwd);

        setNetworkStatus(ap_enabled ? NETWORK_UP : NETWORK_DOWN);
    }
}

void init(NetworkMode mode, const char *host, uint8_t tpw) {
    system_phy_set_max_tpw(tpw);
    WiFi.hostname(host);
    setMode(mode);
}

void setBroadcast(uint8_t _new) {
    // 1: station 2: soft-AP 3: both station and soft-AP
    print_ident(&DEBUG, FPSTR(str_wifi));
    print(&DEBUG, FPSTR(str_broadcast));
    if (wifi_set_broadcast_if(_new))
        println(&DEBUG, _new);
    else
        println(&DEBUG, FPSTR(str_failed));
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

String hostAP_Name() {
    return String("smartpower2");
}

String hostSTA_StatusStr() {
    PGM_P strP;
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
        default:
            strP = str_unknown;
            break;
    }
    return FPSTR(strP);
}

size_t printDiag(Print *p) {
    size_t n = print(p, FPSTR(str_network), getUpDownStr(hasNetwork()));
    n += println(p, FPSTR(str_for), millis_since(hasNetwork() ? lastDown : lastUp));
    n += println_nameP_value(p, str_mode, getMode());
    WiFi.printDiag(*p);
    return n;
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
    char ch;
    switch (WiFi.getPhyMode()) {
        case WIFI_PHY_MODE_11B:
            ch = 'b';
            break;
        case WIFI_PHY_MODE_11G:
            ch = 'g';
            break;
        case WIFI_PHY_MODE_11N:
            ch = 'n';
            break;
        default:
            ch = '?';
            break;
    }
    return String("801.11") + ch;
}

}  // namespace Wireless

// https://github.com/esp8266/Arduino/issues/4114
class WiFiStationStaticIP : public ESP8266WiFiSTAClass {
   public:
    void useStaticStationIP(bool enabled) { _useStaticIp = enabled; }
};

void enableStaticStationIP(bool enabled) {
    WiFiStationStaticIP tmp;
    tmp.useStaticStationIP(enabled);
}

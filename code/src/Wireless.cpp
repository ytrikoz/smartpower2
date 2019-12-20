#include "Wireless.h"

#include "Global.h"

using namespace PrintUtils;
using namespace StrUtils;

namespace Wireless {

Print *out_ = 0;

bool ap_enabled = false;

NetworkMode networkMode = NETWORK_OFF;

NetworkStatus networkStatus = NETWORK_DOWN;

unsigned long lastUp, lastDown = 0;

WiFiEventHandler staGotIpEventHandler, staConnectedEventHandler,
    staDisconnectedEventHandler;

WiFiEventHandler stationConnectedHandler, stationDisconnectedHandler,
    probeRequestHandler;

NetworkStatusChangeEventHandler statusChangeHandler;

void setOutput(Print *p) { out_ = p; }

String AP_Name() { return String(APP_NAME); }

String AP_SSID() { return WiFi.softAPSSID(); }

String AP_Password() { return WiFi.softAPPSK(); }

IPAddress AP_IP() { return WiFi.softAPIP(); }

uint8_t AP_Clients() {
    return (networkMode == NetworkMode::NETWORK_AP ||
            networkMode == NetworkMode::NETWORK_AP_STA)
               ? wifi_softap_get_station_num()
               : 0;
}

String hostSTA_RSSI() { return StrUtils::prettyRssi(WiFi.RSSI()); }

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
            res = Wireless::AP_SSID();
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
            str += Wireless::AP_Name();
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
    IPAddress res;
    switch (getMode()) {
        case NETWORK_AP:
            res = Wireless::AP_IP();
            break;
        case NETWORK_STA:
        case NETWORK_AP_STA:
            res = hostSTA_IP();
            break;
        default:
            res = IPADDR_NONE;
    }
    return res;
}

void setMode(const NetworkMode mode) {
    WiFi.mode((WiFiMode_t)mode);
    delay(100);
}

void setupAP(const IPAddress host) {
    const IPAddress gateway(host);
    const IPAddress subnet(255, 255, 255, 0);

    PrintUtils::print_ident(out_, FPSTR(str_wifi_ap));
    PrintUtils::print_network(out_, host, subnet, gateway);
    PrintUtils::println(out_);

    WiFi.softAPConfig(host, gateway, subnet);
}

void setupSTA() {
    const IPAddress any(IP_ADDR_ANY);
    setupSTA(any, any, any, any);
}

void setupSTA(const IPAddress ipaddr, const IPAddress subnet,
              const IPAddress gateway, const IPAddress dns) {
    PrintUtils::print_ident(out_, FPSTR(str_wifi_sta));
    if (ipaddr == IPAddress(IP4_ADDR_ANY)) {
        PrintUtils::print(out_, FPSTR(str_dhcp));
    } else {
        PrintUtils::print_network(out_, ipaddr, subnet, gateway, dns);
    }
    PrintUtils::println(out_);
    WiFi.config(ipaddr, gateway, subnet, dns);
}

bool startSTA(const char *ssid, const char *passwd) {
    WiFi.begin(ssid, passwd);
    uint8_t result = WiFi.waitForConnectResult();
    PrintUtils::print_ident(out_, FPSTR(str_wifi_sta));
    PrintUtils::print(out_, FPSTR(str_ssid), ssid, getSTAStatus());
    PrintUtils::println(out_);
    return result;
}

bool startAP(const char *ssid, const char *passwd) {
    bool res = WiFi.softAP(ssid, passwd);
    PrintUtils::print_ident(out_, FPSTR(str_wifi_ap));
    if (res) {
        PrintUtils::print(out_, FPSTR(str_ssid), WiFi.softAPSSID());
    } else {
        PrintUtils::print(out_, FPSTR(str_failed));
    }
    PrintUtils::println(out_);
    return res;
}

boolean disconnectWiFi() { return WiFi.disconnect(); }

void start_safe() {
    setMode(NETWORK_AP);
    setupAP(StrUtils::atoip(config->get()->getValueDefault(AP_IPADDR)));
    startAP(SysInfo::getUniqueName().c_str(), config->get()->getValueDefault(AP_PASSWORD));
}

void start() {
    String host = AP_Name();
    uint8_t tpw = config->get()->getValueAsByte(TPW);
    NetworkMode mode = (NetworkMode)config->get()->getValueAsByte(WIFI);

    init(mode, host.c_str(), tpw);

    stationConnectedHandler = WiFi.onSoftAPModeStationConnected(
        [](const WiFiEventSoftAPModeStationConnected &e) {
            PrintUtils::print_wifi_ap_station(out_, e.aid, e.mac, true);
        });

    stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(
        [](const WiFiEventSoftAPModeStationDisconnected &e) {
            PrintUtils::print_wifi_ap_station(out_, e.aid, e.mac, false);
        });

    probeRequestHandler = WiFi.onSoftAPModeProbeRequestReceived(
        [](const WiFiEventSoftAPModeProbeRequestReceived &e) {
           // PrintUtils::print_wifi_ap_probe(out_, e.mac, e.rssi);
        });

    staConnectedEventHandler =
        WiFi.onStationModeConnected([](const WiFiEventStationModeConnected &e) {
            PrintUtils::print_ident(out_, FPSTR(str_wifi_sta));
            PrintUtils::print(out_, FPSTR(str_bssid), StrUtils::prettyMac(e.bssid));
            PrintUtils::print(out_, FPSTR(str_ch), e.channel);
            PrintUtils::println(out_);
        });

    staGotIpEventHandler =
        WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &e) {
            PrintUtils::print_ident(out_, FPSTR(str_wifi_sta));
            PrintUtils::print(out_, FPSTR(str_got));
            PrintUtils::print_network(out_, e.ip, e.mask, e.gw);
            PrintUtils::println(out_);
            updateState();
        });

    staDisconnectedEventHandler = WiFi.onStationModeDisconnected(
        [](const WiFiEventStationModeDisconnected &e) {
            if (networkStatus == NETWORK_UP) {
                PrintUtils::print_ident(out_, FPSTR(str_wifi_sta));
                PrintUtils::print(out_, FPSTR(str_disconnected));
                PrintUtils::println(out_);
            }
            updateState();
        });

    if (mode == NETWORK_STA || mode == NETWORK_AP_STA) {
        const char *ssid = config->get()->getValue(SSID);
        const char *passwd = config->get()->getValue(PASSWORD);
        const bool dhcp = config->get()->getValueAsBool(DHCP);

        if (dhcp) {
            setupSTA();
        } else {
            IPAddress ip, subnet, gateway, dns;
            ip = config->get()->getValueAsIPAddress(IPADDR);
            subnet = config->get()->getValueAsIPAddress(NETMASK);
            gateway = config->get()->getValueAsIPAddress(GATEWAY);
            dns = config->get()->getValueAsIPAddress(DNS);
            setupSTA(ip, gateway, subnet, dns);
        }

        if (startSTA(ssid, passwd)) {
            if (mode == NETWORK_AP_STA)
                setBroadcast(3);
        }
    }

    if (mode == NETWORK_AP || mode == NETWORK_AP_STA) {
        const char *ap_ssid = config->get()->getValue(ConfigItem::AP_SSID);
        const char *ap_passwd = config->get()->getValue(ConfigItem::AP_PASSWORD);
        IPAddress ap_ipaddr = config->get()->getValueAsIPAddress(ConfigItem::AP_IPADDR);

        setupAP(ap_ipaddr);

        ap_enabled = startAP(ap_ssid, ap_passwd);

        updateState();
    }
}

void init(NetworkMode mode, const char *host, uint8_t tpw) {
    PrintUtils::print_ident(out_, FPSTR(str_init));
    PrintUtils::print(out_, FPSTR(str_host), host);
    PrintUtils::print(out_, FPSTR(str_tpw), tpw);
    PrintUtils::println(out_);
    system_phy_set_max_tpw(tpw);
    WiFi.hostname(host);
    setMode(mode);
}

void setBroadcast(uint8_t _if) {
    // 1: station 2: soft-AP 3: both station and soft-AP
    PrintUtils::print_ident(out_, FPSTR(str_wifi));
    PrintUtils::print(out_, FPSTR(str_broadcast), _if);
    if (!wifi_set_broadcast_if(_if))
        PrintUtils::print(out_, FPSTR(str_failed));
    PrintUtils::println(out_);
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

bool hasNetwork() {
    return networkStatus == NETWORK_UP;
}

bool isActive() {
    return WiFi.softAPgetStationNum() || WiFi.status() == WL_CONNECTED;
}

void setOnNetworkStatusChange(NetworkStatusChangeEventHandler h) {
    statusChangeHandler = h;
    if (statusChangeHandler)
        statusChangeHandler(hasNetwork(), millis());
}

String getNetworkState() {
    bool has = hasNetwork();
    String str = FPSTR(str_network);
    str += getUpDownStr(has);
    str += millis_since(has ? lastDown : lastUp);
    return str;
}

String getSTAStatus() {
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
    return String(FPSTR(strP));
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

String wifiChannel() {
    return String(WiFi.channel());
}

String wifiPhyMode() {
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
#include "Wireless.h"

#include "Global.h"

void Wireless::setOutput(Print *p) { out_ = p; }

void Wireless::setMode(const NetworkMode mode) {
    WiFi.mode((WiFiMode_t)mode);
    delay(100);
}

void Wireless::setupAP(const IPAddress host) {
    const IPAddress gateway(host);
    const IPAddress subnet(255, 255, 255, 0);

    PrintUtils::print_ident(out_, FPSTR(str_wifi_ap));
    PrintUtils::print_network(out_, host, subnet, gateway);
    PrintUtils::println(out_);

    WiFi.softAPConfig(host, gateway, subnet);
}

void Wireless::setupSTA(const IPAddress ipaddr, const IPAddress subnet,
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

bool Wireless::startSTA(const char *ssid, const char *passwd) {
    WiFi.begin(ssid, passwd);
    uint8_t res = WiFi.waitForConnectResult();
    PrintUtils::print_ident(out_, FPSTR(str_wifi_sta));
    PrintUtils::print(out_, FPSTR(str_ssid), ssid, NetUtils::getStaStatus());
    PrintUtils::println(out_);
    return res;
}

bool Wireless::startAP(const char *ssid, const char *passwd) {
    bool res = WiFi.softAP(ssid, passwd);
    PrintUtils::print_ident(out_, FPSTR(str_wifi_ap));
    if (res)
        PrintUtils::print(out_, FPSTR(str_ssid), WiFi.softAPSSID());
    else
        PrintUtils::print(out_, FPSTR(str_failed));
    PrintUtils::println(out_);
    return res;
}

boolean Wireless::disconnectWiFi() { return WiFi.disconnect(); }

void Wireless::start(const bool safe) {
    if (safe) {
        setMode(NETWORK_AP);
        setupAP(StrUtils::str2ip(config->get()->getDefault(AP_IPADDR)));
        startAP(SysInfo::getUniqueName().c_str(), config->get()->getDefault(AP_PASSWORD));
        return;
    }

    uint8_t tpw = config->get()->asByte(TPW);
    NetworkMode mode = (NetworkMode)config->get()->asByte(WIFI);
    init(mode, APP_NAME, tpw);

    stationConnectedHandler = WiFi.onSoftAPModeStationConnected(
        [this](const WiFiEventSoftAPModeStationConnected &e) {
            PrintUtils::print_wifi_ap_station(out_, e.aid, e.mac, true);
        });

    stationDisconnectedHandler = WiFi.onSoftAPModeStationDisconnected(
        [this](const WiFiEventSoftAPModeStationDisconnected &e) {
            PrintUtils::print_wifi_ap_station(out_, e.aid, e.mac, false);
        });

    probeRequestHandler = WiFi.onSoftAPModeProbeRequestReceived(
        [this](const WiFiEventSoftAPModeProbeRequestReceived &e) {
            // PrintUtils::print_wifi_ap_probe(out_, e.mac, e.rssi);
        });

    staConnectedEventHandler =
        WiFi.onStationModeConnected([this](const WiFiEventStationModeConnected &e) {
            PrintUtils::print_ident(out_, FPSTR(str_wifi_sta));
            PrintUtils::print(out_, FPSTR(str_bssid), StrUtils::prettyMac(e.bssid));
            PrintUtils::print(out_, FPSTR(str_ch), e.channel);
            PrintUtils::println(out_);
        });

    staGotIpEventHandler =
        WiFi.onStationModeGotIP([this](const WiFiEventStationModeGotIP &e) {
            PrintUtils::print_ident(out_, FPSTR(str_wifi_sta));
            PrintUtils::print_network(out_, e.ip, e.mask, e.gw);
            PrintUtils::println(out_);
            refreshStatus();
        });

    staDisconnectedEventHandler = WiFi.onStationModeDisconnected(
        [this](const WiFiEventStationModeDisconnected &e) {
            if (networkStatus_ == NETWORK_UP) {
                PrintUtils::print_ident(out_, FPSTR(str_wifi_sta));
                PrintUtils::print(out_, FPSTR(str_disconnected));
                PrintUtils::print(out_, FPSTR(str_reason), e.reason);
                PrintUtils::println(out_);
                refreshStatus();
            }
        });

    if (mode == NETWORK_AP || mode == NETWORK_AP_STA) {
        const char *ap_ssid = config->get()->value(ConfigItem::AP_SSID);
        const char *ap_passwd = config->get()->value(ConfigItem::AP_PASSWORD);
        IPAddress ap_ipaddr = config->get()->asIPAddress(ConfigItem::AP_IPADDR);
        setupAP(ap_ipaddr);
        ap_enabled_ = startAP(ap_ssid, ap_passwd);
    }

    if (mode == NETWORK_STA || mode == NETWORK_AP_STA) {
        const char *ssid = config->get()->value(ConfigItem::SSID);
        const char *passwd = config->get()->value(ConfigItem::PASSWORD);
        const bool dhcp = config->get()->asBool(ConfigItem::DHCP);
        IPAddress ip, subnet, gateway, dns;
        if (!dhcp) {
            ip = config->get()->asIPAddress(ConfigItem::IPADDR);
            subnet = config->get()->asIPAddress(ConfigItem::NETMASK);
            gateway = config->get()->asIPAddress(ConfigItem::GATEWAY);
            dns = config->get()->asIPAddress(ConfigItem::DNS);
        }
        setupSTA(ip, gateway, subnet, dns);

        if (startSTA(ssid, passwd)) {
            if (mode == NETWORK_AP_STA) setBroadcast(3);
        }        
    }

    refreshStatus();
}

void Wireless::init(const NetworkMode mode, const char *host, uint8_t tpw) {
    PrintUtils::print_ident(out_, FPSTR(str_wifi));
    PrintUtils::print(out_, FPSTR(str_host), host);
    PrintUtils::print(out_, FPSTR(str_tpw), tpw);
    PrintUtils::println(out_);
    system_phy_set_max_tpw(tpw);
    WiFi.hostname(host);
    setMode(mode);
}

void Wireless::setBroadcast(uint8_t _if) {
    // 1: station 2: soft-AP 3: both station and soft-AP
    PrintUtils::print_ident(out_, FPSTR(str_wifi));
    PrintUtils::print(out_, FPSTR(str_broadcast), _if);
    if (!wifi_set_broadcast_if(_if))
        PrintUtils::print(out_, FPSTR(str_failed));
    PrintUtils::println(out_);
}

NetworkMode Wireless::getMode() {
    return (NetworkMode)WiFi.getMode();
}

void Wireless::refreshStatus() {
    bool res = false;
    switch (WiFi.getMode()) {
        case WiFiMode_t::WIFI_OFF:
            break;
        case WiFiMode_t::WIFI_AP:
            res = ap_enabled_;
            break;
        case WiFiMode_t::WIFI_STA:
        case WiFiMode_t::WIFI_AP_STA:
            res = WiFi.isConnected() || ap_enabled_;
            break;
        default:
            break;
    }
    setStatus(res ? NETWORK_UP : NETWORK_DOWN);
}

void Wireless::setStatus(NetworkStatus status) {
    unsigned long now = millis();
    if (networkStatus_ != status) {
        if (networkStatus_ == NETWORK_UP) lastUp_ = now;
        if (networkStatus_ == NETWORK_DOWN) lastDown_ = now;
        networkStatus_ = status;
        statusChangeEvent(now);
    }
}

void Wireless::setOnStatusChange(NetworkStatusChangeEventHandler h) {
    statusChangeHandler = h;
    statusChangeEvent();
}

void Wireless::statusChangeEvent(unsigned long now) {
    if (statusChangeHandler)
        statusChangeHandler(hasNetwork(), millis_passed(networkStatus_ == NETWORK_UP ? lastDown_ : lastUp_, now));
}

bool Wireless::hasNetwork() {
    return networkStatus_ == NETWORK_UP;
}

bool Wireless::isActive() {
    return WiFi.softAPgetStationNum() || WiFi.status() == WL_CONNECTED;
}

bool Wireless::isScanning() {
    return scanning_;
}

void Wireless::startWiFiScan(bool hidden) {
    WiFi.scanNetworksAsync([this](int num) { onScanComplete(num); }, hidden);
    scanning_ = true;
}

void Wireless::onScanComplete(int found) {
    Wireless::scanning_ = false;
    File f = LittleFS.open("/var/networks", "w");
    if (found > 0) {
        for (int i = 0; i < found; ++i) {
            f.printf("%-18s", WiFi.BSSIDstr(i).c_str());
            f.printf("%-4s", "");
            f.printf("%-20s", WiFi.SSID(i).c_str());
            f.printf("%-6d", WiFi.RSSI(i));
            f.printf("%-6s", NetUtils::encryptTypeStr(i));
            f.printf("%4d", WiFi.channel(i));
            f.println();
        };
    }
    f.close();
}

// https://github.com/esp8266/Arduino/issues/4114
class WiFiStationStaticIP : public ESP8266WiFiSTAClass {
   public:
    void useStaticStationIP(bool enabled) { _useStaticIp = enabled; }
};

void enableStaticStationIP(bool enabled) {
    WiFiStationStaticIP tmp;
    tmp.useStaticStationIP(enabled);
};

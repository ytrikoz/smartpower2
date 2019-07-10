#include "ip_utils.h"

IPAddress atoip(const char *input) {
    uint8_t parts[4] = {0, 0, 0, 0};
    uint8_t part = 0;
    for (uint8_t a = 0; a < strlen(input); a++) {
        uint8_t b = input[a];
        if (b == '.') {
            part++;
            continue;
        }
        parts[part] *= 10;
        parts[part] += b - '0';
    }
    return IPAddress(parts[0], parts[1], parts[2], parts[3]);
}

String iptoa(IPAddress ip) {
    String res = "";
    for (int i = 0; i < 3; i++) {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);

    return res;
}

bool ipvalid(const char *input) {
    for (size_t i = 0; i < strlen(input); i++) {
        int c = input[i];
        if (c != '.' && (c < '0' || c > '9')) return false;
    }
    return true;
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

bool startAP(const char *ssid, const char *password, IPAddress host) {
    setupAP(host);

    bool result = false;

    if (strlen(password) < 8) {
        result = WiFi.softAP(ssid);
    } else {
        result = WiFi.softAP(ssid, password);
    }

    return result;
}

void startSTA(const char *ssid, const char *passwd) {
    startSTA(ssid, passwd, IPAddress(IP_ADDR_ANY), IPAddress(IP_ADDR_ANY),
             IPAddress(IP_ADDR_ANY), IPAddress(IP_ADDR_ANY));
}

void startSTA(const char *ssid, const char *password, IPAddress host,
              IPAddress subnet, IPAddress gateway, IPAddress dns) {
    setupSTA(host, gateway, subnet, dns);

    WiFi.begin(ssid, password);
}

void setupAP(IPAddress host) {
    WiFi.mode(WIFI_AP);
    WiFi.disconnect();
    delay(100);
    WiFi.softAPConfig(host, host, IPAddress(255, 255, 255, 0));
}

void setupSTA(IPAddress host, IPAddress gateway, IPAddress subnet,
              IPAddress dns) {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.config(host, gateway, subnet, dns);
}

String hostSSID() {
    String str;
    if (WiFi.getMode() == WIFI_STA) {
        str = WiFi.SSID();
    } else {
        str = WiFi.softAPSSID();
    }
    return str;
}

IPAddress hostIP() {
    switch (WiFi.getMode()) {
        case WIFI_STA:
            return WiFi.localIP();
            break;
        case WIFI_AP:
        case WIFI_AP_STA:
            return WiFi.softAPIP();
            ;
            break;
        default:
            return IPAddress(IPADDR_NONE);
    }
}

String getFormatedSocketStr(IPAddress ip, int port) {
    char str[32];
    if (ip.isSet()) {
        char buf[4];
        for (int i = 0; i < 4; i++) {
            strcpy(str, itoa(ip[i], buf, DEC));
            if (i != 3) {
                uint8_t len = strlen(str);
                str[len] = '.';
                str[len + 1] = '\x00';
            }
        }
    } else {
        strcpy(str, "(unset)");
    }

    uint8_t len = strlen(str);
    str[len] = ':';
    str[len + 1] = '\x00';

    char buf[8];
    strcpy(str, itoa(port, buf, DEC));

    return String(str);
}

#pragma once

#include <ESP8266WiFi.h>

#include "Strings.h"
#include "Utils/StrUtils.h"

namespace NetUtils {

inline size_t getSoftApClients() {
    return wifi_softap_get_station_num();
}

inline String getRssi() {
    return StrUtils::prettyRssi(WiFi.RSSI());
}

inline IPAddress getStaIp() {
    return WiFi.localIP();
}

inline String getStaSsid() {
    return WiFi.SSID();
}

inline String getStatusStr(bool network) {
    String str = FPSTR(str_network);
    str += ' ';
    str += StrUtils::bool2str(network, StrUtils::UP_DONW);
    return str;
}

inline String getStaStatus() {
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

inline String getMode(NetworkMode mode) {
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
    return String(FPSTR(strP));
}

inline String getMode() {
    return getMode((NetworkMode)WiFi.getMode());
}

inline String getWifiPhy() {
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
    return "801.11" + ch;
}

inline const char *encryptTypeStr(const int type) {
    switch (type) {
        case AUTH_OPEN:
            return "none";
        case AUTH_WEP:
            return "WEP";
        case AUTH_WPA_PSK:
            return "WPA";
        case AUTH_WPA2_PSK:
            return "WPA2";
        case AUTH_WPA_WPA2_PSK:
            return "auto";
        default:
            return "---";
    }
}

inline String getApSsid() {
    return WiFi.softAPSSID();
}

inline NetInfo getNetInfo(uint8 if_index) {
    struct ip_info ip;
    wifi_get_ip_info(if_index, &ip);
    return NetInfo(ip);
}

inline String getApPasswd() {
    return WiFi.softAPPSK();
}

inline IPAddress getApIp() {
    return WiFi.softAPIP();
}

inline String getApMac() {
    return WiFi.softAPmacAddress();
}

inline String getWifiCh() {
    return String(WiFi.channel(), DEC);
}

}  // namespace WirelessUtils

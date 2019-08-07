#pragma once

#define DEBUG Serial

//#define SERIAL_DEBUG
//#define DEBUG_CONFIG
//#define DEBUG_DISPLAY
//#define DEBUG_SYSTEM_CLOCK
//#define DEBUG_WIRELESS
//#define DEBUG_NTP
//#define DEBUG_TELNET
//#define DEBUG_WEBSOCKET
#define DEBUG_HTTP
//#define DEBUG_LEDS
//#define DEBUG_SHELL
//#define DEBUG_TERMUL
//#define DEBUG_FILE_STORAGE
//#define DEBUG_TIME_BACKUP

#define PRINTLN_WIFI_NETWORK_DOWN     \
    USE_SERIAL.printf_P(str_wifi);    \
    USE_SERIAL.printf_P(str_network); \
    USE_SERIAL.printf_P(str_down);    \
    USE_SERIAL.println();

#define PRINTLN_WIFI_NETWORK_UP       \
    USE_SERIAL.printf_P(str_wifi);    \
    USE_SERIAL.printf_P(str_network); \
    USE_SERIAL.printf_P(str_up);      \
    USE_SERIAL.println();

#define PRINTLN_WIFI_SWITCHED_OFF      \
    USE_SERIAL.printf_P(str_wifi);     \
    USE_SERIAL.printf_P(str_switched); \
    USE_SERIAL.printf_P(str_off);      \
    USE_SERIAL.println();

#define PRINT_SWITCHED_ON              \
    USE_SERIAL.printf_P(str_switched); \
    USE_SERIAL.printf_P(str_on);

#define PRINT_IP                                                               \
    USE_SERIAL.printf_P(strf_ip_params, ip.toString().c_str(),                 \
                        subnet.toString().c_str(), gateway.toString().c_str(), \
                        dns.toString().c_str());

#define PRINT_WIFI_STA             \
    USE_SERIAL.printf_P(str_wifi); \
    USE_SERIAL.printf_P(str_sta);

#define PRINTLN_WIFI_STA_CONNECTED      \
    PRINT_WIFI_STA                      \
    USE_SERIAL.printf_P(str_connected); \
    PRINT_WIFI_STA_CONNECTION           \
    USE_SERIAL.println();

#define PRINTLN_WIFI_CONFIG                    \
    USE_SERIAL.printf_P(str_wifi);             \
    USE_SERIAL.printf_P(strf_mode, mode);      \
    USE_SERIAL.printf_P(strf_tpw, tpw);        \
    USE_SERIAL.printf_P(strf_host, HOST_NAME); \
    USE_SERIAL.println();

#define PRINT_WIFI_AP              \
    USE_SERIAL.printf_P(str_wifi); \
    USE_SERIAL.printf_P(str_ap);

#define PRINT_WIFI_AP_CONFIG                 \
    USE_SERIAL.printf_P(strf_ssid, ap_ssid); \
    USE_SERIAL.printf_P(strf_ipaddr, ap_ipaddr.toString().c_str());

#define PRINT_WIFI_STA_CONNECTION                                       \
    USE_SERIAL.printf_P(strf_bssid, e.bssid[0], e.bssid[1], e.bssid[2], \
                        e.bssid[3], e.bssid[4], e.bssid[5]);            \
    USE_SERIAL.printf_P(strf_channel, e.channel);                       \
    USE_SERIAL.printf_P(str_ssid);                                      \
    USE_SERIAL.print(e.ssid);

#define PRINTLN_WIFI_STA_DISCONNECTED      \
    PRINT_WIFI_STA                         \
    USE_SERIAL.printf_P(str_disconnected); \
    USE_SERIAL.println();

#define PRINTLN_WIFI_STA_GOT_IP   \
    PRINT_WIFI_STA                \
    USE_SERIAL.printf_P(str_got); \
    PRINT_IP                      \
    USE_SERIAL.println();

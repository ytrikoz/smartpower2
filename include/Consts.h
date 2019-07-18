#pragma once

#include <pgmspace.h>

#define NO_GLOBAL_SSDP
#define NO_GLOBAL_MDNS
#define NO_GLOBAL_NBNS
#define NO_GLOBAL_NETBIOS
#define NO_GLOBAL_ARDUINOOTA

//#define SERIAL_DEBUG

//#define DEBUG_WIRELESS
//#define DEBUG_NTP
//#define DEBUG_TELNET
//#define DEBUG_SYSTEM_CLOCK
#define DEBUG_WEBSOCKET
//#define DEBUG_HTTP
//#define DEBUG_LEDS
//#define DEBUG_SHELL
//#define DEBUG_CONFIG
//#define DEBUG_FILE_STORAGE
//#define DEBUG_TIME_BACKUP
//#define DEBUG_LCD

#define USE_DEBUG_SERIAL Serial

#define USE_SERIAL Serial

//#define DISABLE_HTTP
//#define DISABLE_LCD
//#define DISABLE_TIME_BACKUP
//#define DISABLE_TELNET
//#define DISABLE_TELNET_SHELL
//#define DISABLE_SERIAL_SHELL
//#define DISABLE_OTA_UPDATE
//#define DISABLE_NTP
//#define DISABLE_NETWORK_DISCOVERY

#define APPNAME "SmartPower2"
#define FW_VERSION \
    "0.6.2"        \
    " (" __DATE__ ")"
#define BUILD_DATE_TIME __DATE__ __TIME__

#define LOOP_STATS_INTERVAL 5000UL

#define START_SHOWING_RESTART_COUNTER_ON 5
#define ONE_MHz_hz 1000000UL
#define ONE_SECOND_ms 1000
#define ONE_MINUTE_s 60
#define ONE_HOUR_s 3600
#define MEASUREMENT_INTERVAL_ms 50
#define ONE_HOUR_ms ONE_HOUR_s* ONE_SECOND_ms;
#define SEVENTY_YEARS_ms 2208988800UL

#define FILENAME_MAX_LENGTH 31
#define FILE_CONFIG "/cfg/settings.ini"
#define FILE_LAST_POWER_STATE "/cfg/powerstate"
#define FILE_TIME_BACKUP "/cfg/clock"
#define FILE_WEB_SETTINGS "/www/js/settings.js"

#define HOST_NAME "smartpower2"
#define HOST_DOMAIN "*"
#define WIFI_CONNECT_TIMEOUT 40
#define MAX_WEB_CLIENTS 5
#define OTA_PORT 8266
#define DNS_PORT 53
#define HTTP_PORT 80
#define WEBSOCKET_PORT 81
#define TELNET_PORT 23
#define WEB_ROOT "/www"
#define NTP_REMOTE_PORT 123
#define NTP_LOCAL_PORT 2390

#define DEF_NTP_POOL_SERVER "pool.ntp.org"
#define TIME_BACKUP_INTERVAL_MIN_s 600

#define DISPLAY_VIRTUAL_COLS 63
#define DISPLAY_VIRTUAL_ROWS 4

#define SCREEN_WIDTH 80
#define INPUT_MAX_LENGTH 128
#define OUTPUT_MAX_LENGTH 128

// slave address are 0x27 or 0x3f
#define LCD_SLAVE_ADDRESS 0x3f

#define CHR_ZERO '\x30'
#define CHAR_NULL '\x00'
#define CHAR_BEL '\x07'
#define CHAR_BS '\x08'
#define CHAR_TAB '\x09'
#define CHAR_LF '\x0a'
#define CHAR_LT '\x8b'
#define CHAR_CR '\x0d'
#define CHAR_SP '\x20'
#define CHAR_DEL '\x7f'
#define CHAR_BIN '\xFF'
#define CHAR_CSI '\x9b'
#define CHAR_ESC '\x1b'

#define ESC_CURSOR_HOME "\x1b[H"
#define ESC_SAVE_CURSOR "\x1b[s"
#define ESC_UNSAVE_CURSOR "\x1b[u"
#define ESC_SAVE_CURSOR_AND_ATTRS "\x1b[7"
#define ESC_RESTORE_CURSOR_AND_ATTRS "\x1b[8"

#define ESC_CLEAR "\x1b[2J"
#define ESC_CLEAR_BOTTOM "\x1b[J"
#define ESC_CLEAR_EOL "\x1b[0K"

#define ESCF_CURSOR_BACKWARD "\x1b[%dD"
#define ESC_CURSOR_UP "\x1b[1A"
#define ESC_CURSOR_DOWN "\x1b[1B"
#define ESC_CURSOR_FORWARD "\x1b[1C"
#define ESC_CURSOR_BACKWARD "\x1b[1D"

static const char strf_http_file_not_found[] PROGMEM =
    "File Not Found\n\nURI: %s\nMethod: %s\nArguments: %d\n";
static const char strf_time[] PROGMEM = "%02d:%02d:%02d";
static const char strf_s_d[] PROGMEM = "%s:%d ";
static const char strf_arrow_dest[] PROGMEM = "-> %s";
static const char strf_arrow_src[] PROGMEM = "<- %s";
static const char strf_binnary[] PROGMEM = "binnary %s";
static const char strf_http_params[] PROGMEM = "%s <- %s:%d(%d)";
static const char strf_wifi_params[] PROGMEM = "ssid %s ip %s ";
static const char strf_wifi_scan_results[] PROGMEM = "#%d %s %d";
static const char strf_file_print[] PROGMEM = "Print '%s'";
static const char strf_file_deleted[] PROGMEM = "File '%s' deleted";
static const char strf_file_not_found[] PROGMEM = "File '%s' not found";
static const char strf_config_param_value[] PROGMEM = "'%s'='%s'";
static const char strf_show_status[] PROGMEM = "lps %lu max %lums ";
static const char strf_timezone[] PROGMEM = "timezone %d ";
static const char strf_mode[] PROGMEM = "mode %d ";
static const char strf_host[] PROGMEM = "hostname %s ";
static const char strf_tpw[] PROGMEM = "tpw %d ";
static const char strf_config_param_changed[] PROGMEM =
    "%s сhanged '%s' -> '%s'";
static const char strf_unknown_command_item[] PROGMEM =
    "Unknown item '%s' for command '%s'";
static const char strf_unknown_action_param[] PROGMEM =
    "Unknown param '%s' for action '%s'";
static const char strf_config_unknown_param[] PROGMEM = "Unknown param '%s'";
static const char strf_set_s[] PROGMEM = "set %s";
static const char strf_progress[] PROGMEM = " %u%%\r";
static const char strf_boot_progress[] PROGMEM = "%s %u%%";
static const char strf_client[] PROGMEM = "#%d ";
static const char strf_set_broadcast[] PROGMEM = "set broadcast if%d to if%d ";
static const char strf_ip_params[] PROGMEM =
    "ip %s subnet %s gateway %s dns %s";
static const char strf_mac[] PROGMEM = "mac %02x:%02x:%02x:%02x:%02x:%02x ";
static const char strf_bssid[] PROGMEM = "bssid %02x:%02x:%02x:%02x:%02x:%02x ";
static const char strf_channel[] PROGMEM = "ch %d ";
static const char strf_port[] PROGMEM = "port %d ";
static const char strf_ssid[] PROGMEM = "ssid %s ";
static const char strf_heap[] PROGMEM = "heap %s ";
static const char strf_passwd[] PROGMEM = "passwd %s ";
static const char strf_ipaddr[] PROGMEM = "ipaddr %s ";
static const char strf_unhandled[] PROGMEM = "unhandled %d";
static const char strf_ntp[] PROGMEM = "[ntp] %s";
static const char strf_output_voltage[] PROGMEM = "output voltage %2.2f";

static const char str_dns[] PROGMEM = "[dns] ";
static const char str_mdns[] PROGMEM = "[mdns] ";
static const char str_ssid[] PROGMEM = "ssid ";
static const char str_success[] PROGMEM = "success ";
static const char str_done[] PROGMEM = "done ";
static const char str_error[] PROGMEM = "error ";
static const char str_start[] PROGMEM = "start ";
static const char str_stopped[] PROGMEM = "stopped ";
static const char str_failed[] PROGMEM = "failed ";
static const char str_ready[] PROGMEM = "ready";
static const char str_firmware[] PROGMEM = "firmware";
static const char str_spiffs[] PROGMEM = "spiffs";
static const char str_unknown[] PROGMEM = "unknown";
static const char str_complete[] PROGMEM = "complete";
static const char str_unset[] PROGMEM = "<unset> ";
static const char str_invalid[] PROGMEM = "<invalid>";
static const char str_unsecured[] PROGMEM = "<unsecured>";
static const char str_host[] PROGMEM = "host ";
static const char str_arrow_dest[] PROGMEM = "-> ";
static const char str_lcd[] PROGMEM = "[lcd] ";
static const char str_ntp[] PROGMEM = "[ntp] ";
static const char str_power[] PROGMEM = "[power] ";
static const char str_http[] PROGMEM = "[http] ";
static const char str_netbios[] PROGMEM = "[netbios] ";
static const char str_update[] PROGMEM = "[update] ";
static const char str_clock[] PROGMEM = "[clock] ";
static const char str_telnet[] PROGMEM = "[telnet] ";
static const char str_wifi[] PROGMEM = "[wifi] ";
static const char str_sta[] PROGMEM = "sta ";
static const char str_disabled[] PROGMEM = "disabled ";
static const char str_twp[] PROGMEM = "twp ";
static const char str_got[] PROGMEM = "got ";
static const char str_ap[] PROGMEM = "ap ";
static const char str_switched[] PROGMEM = "switched ";
static const char str_on[] PROGMEM = "on ";
static const char str_off[] PROGMEM = "off ";
static const char str_dhcp_on[] PROGMEM = " dhcp on ";
static const char str_restore[] PROGMEM = "restore ";
static const char str_store[] PROGMEM = "store ";
static const char str_synced[] PROGMEM = "synced ";
static const char str_connected[] PROGMEM = "connected ";
static const char str_disconnected[] PROGMEM = "disconnected ";
static const char str_up[] PROGMEM = "up ";
static const char str_down[] PROGMEM = "down ";
static const char str_mode[] PROGMEM = "mode ";
static const char str_network[] PROGMEM = "network ";
static const char str_scanning[] PROGMEM = "scanning... ";
static const char str_session_interrupted[] PROGMEM =
    "Session interrupted by another connection";
static const char str_avaible_system_actions[] PROGMEM =
    "Available actions are: reset, load, save, restart, power";
static const char str_network_not_found[] PROGMEM = "no networks found";
static const char str_network_found[] PROGMEM = "%d networks found";
static const char str_cli_hint[] PROGMEM = "[cli] press \"enter\" to start";

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

typedef enum { EMPTY } Dummy;
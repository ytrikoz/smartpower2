#pragma once

#include <pgmspace.h>

#define NO_GLOBAL_SSDP
#define NO_GLOBAL_MDNS
#define NO_GLOBAL_NBNS
#define NO_GLOBAL_NETBIOS
#define NO_GLOBAL_ARDUINOOTA

#define USE_SERIAL Serial

//#define DISABLE_HTTP
//#define DISABLE_LCD
//#define DISABLE_TIME_BACKUP
//#define DISABLE_TELNET
//#define DISABLE_TELNET_SHELL
//#define DISABLE_CONSOLE_SHELL
//#define DISABLE_OTA_UPDATE
//#define DISABLE_NTP
//#define DISABLE_NETWORK_DISCOVERY

#define BUILD_TIMEZONE_h 3
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__
#define APPNAME "SmartPower2"
#define FW_VERSION \
    "0.6.4"        \
    "(" BUILD_DATE ")"
#define DATETIME_FORMAT "%02d/%02d/%04d %02d:%02d:%02d"
#define ONE_MHz_hz 1000000UL
#define ONE_SECOND_ms 1000
#define ONE_MINUTE_s 60
#define ONE_DAY_s 86400
#define ONE_HOUR_s 3600
#define ONE_HOUR_m 60
#define ONE_DAY_h 24
#define ONE_HOUR_ms ONE_HOUR_s* ONE_SECOND_ms;
#define SEVENTY_YEARS_ms 2208988800UL
#define MILLENIUM_s 946684800UL

#define MEASUREMENT_INTERVAL_ms 100
#define START_SHOWING_RESTART_COUNTER_ON 5
#define LOOP_STATS_INTERVAL 5000UL

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
#define DEF_TIME_BACKUP_INTERVAL_s "3600"
#define TIME_BACKUP_INTERVAL_MIN_s 600
#define WOL_PORT 9

#define PSU_LOG_SIZE 128
#define PSU_LOG_INTERVAL_ms 1000

#define DISPLAY_VIRTUAL_COLS 63
#define DISPLAY_VIRTUAL_ROWS 4

#define SCREEN_WIDTH 80
#define INPUT_MAX_LENGTH 128
#define OUTPUT_MAX_LENGTH 128

static const char strf_http_file_not_found[] PROGMEM =
    "File Not Found\n\nURI: %s\nMethod: %s\nArguments: %d\n";
static const char strf_time[] PROGMEM = "%02d:%02d:%02d";
static const char strf_s_d[] PROGMEM = "%s:%d ";
static const char strf_ip_port[] PROGMEM = "%s:%d ";
static const char strf_every_ms[] PROGMEM = "every %lu ms ";
static const char strf_every_sec[] PROGMEM = "every %lu sec ";
static const char strf_arrow_dest[] PROGMEM = "-> %s";
static const char strf_arrow_src[] PROGMEM = "<- %s";
static const char strf_active[] PROGMEM = "active %d";
static const char strf_binnary[] PROGMEM = "binnary %s";
static const char strf_init[] PROGMEM = "init %d";
static const char strf_http_params[] PROGMEM = "%s <- %s:%d(%d)";
static const char strf_wifi_params[] PROGMEM = "ssid %s ip %s ";
static const char strf_wifi_scan_results[] PROGMEM = "#%d %s %d";
static const char strf_file_print[] PROGMEM = "Print '%s'";
static const char strf_file_deleted[] PROGMEM = "File '%s' deleted";
static const char strf_file_not_found[] PROGMEM = "File '%s' not found";
static const char strf_config_param_value[] PROGMEM = "'%s'='%s'";
static const char strf_show_status[] PROGMEM = "lps %lu max %lums ";
static const char strf_synced[] PROGMEM = "synced %d ";
static const char strf_timezone[] PROGMEM = "timezone %d ";
static const char strf_mode[] PROGMEM = "mode %d ";
static const char strf_host[] PROGMEM = "hostname %s ";
static const char strf_tpw[] PROGMEM = "tpw %d ";
static const char strf_ip[] PROGMEM = "ip %s ";
static const char strf_in_second[] PROGMEM = "in %d seconds";
static const char strf_config_param_changed[] PROGMEM =
    "The new value '%s' for the '%s' has been set!";
static const char strf_config_param_unchanged[] PROGMEM = "'%s' unchanged";
static const char strf_unknown_action[] PROGMEM = "unknown action '%s'";
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
static const char strf_epoch[] PROGMEM = "epoch %lu";
static const char strf_interval[] PROGMEM = "interval %d";
static const char strf_output_voltage[] PROGMEM = "output voltage %2.2f";
static const char strf_filelist[] PROGMEM = "filelist %s";
static const char strf_power[] PROGMEM = "power is %s";

static const char str_as_default[] PROGMEM = "as default ";
static const char str_ap[] PROGMEM = "ap ";
static const char str_arrow_dest[] PROGMEM = "-> ";
static const char str_avaible_system_actions[] PROGMEM = "Available actions for 'system' command are: 'reset', 'load', 'save', 'restart'.";
static const char str_backup[] PROGMEM = "backup ";
static const char str_build_date[] PROGMEM = BUILD_DATE;
static const char str_clients[] = "clients";
static const char str_cli_hint[] PROGMEM = "[cli] press \"enter\" to start";
static const char str_clock[] PROGMEM = "[clock] ";
static const char str_config[] PROGMEM = "[config] ";
static const char str_connecting[] PROGMEM = "connecting ";
static const char str_connection[] PROGMEM = "connection ";
static const char str_connected[] PROGMEM = "connected ";
static const char str_complete[] PROGMEM = "complete";
static const char str_date[] PROGMEM = "date ";
static const char str_disabled[] PROGMEM = "disabled ";
static const char str_disconnected[] PROGMEM = "disconnected ";
static const char str_dhcp_on[] PROGMEM = " dhcp on ";
static const char str_dns[] PROGMEM = "[dns] ";
static const char str_done[] PROGMEM = "done ";
static const char str_down[] PROGMEM = "down ";
static const char str_interval[] PROGMEM = "interval ";
static const char str_idle[] PROGMEM = "idle ";
static const char str_invalid[] PROGMEM = "invalid %s";
static const char str_http[] PROGMEM = "[http] ";
static const char str_http_[] PROGMEM = "http";
static const char str_got[] PROGMEM = "got ";
static const char str_last_known[] PROGMEM = "last known ";
static const char str_lcd[] PROGMEM = "[lcd] ";
static const char str_mdns[] PROGMEM = "[mdns] ";
static const char str_mode[] PROGMEM = "mode ";
static const char str_netbios[] PROGMEM = "[netbios] ";
static const char str_network[] PROGMEM = "network ";
static const char str_network_not_found[] PROGMEM = "no networks found";
static const char str_network_found[] PROGMEM = "%d networks found";
static const char str_no[] PROGMEM = "no ";
static const char str_ntp[] PROGMEM = "[ntp] ";
static const char str_error[] PROGMEM = "error ";
static const char str_failed[] PROGMEM = "failed ";
static const char str_firmware[] PROGMEM = "firmware";
static const char str_found[] PROGMEM = "found ";
static const char str_ready[] PROGMEM = "ready";
static const char str_host[] PROGMEM = "host ";
static const char str_on[] PROGMEM = "on ";
static const char str_off[] PROGMEM = "off ";
static const char str_password[] PROGMEM = "password ";
static const char str_psu[] PROGMEM = "[psu] ";
static const char str_up[] PROGMEM = "up ";
static const char str_unknown[] PROGMEM = "unknown";
static const char str_update[] PROGMEM = "[update] ";
static const char str_unset[] PROGMEM = "<unset> ";
static const char str_unsecured[] PROGMEM = "<unsecured>";
static const char str_reconnect[] PROGMEM = "reconnect ";
static const char str_restore[] PROGMEM = "restore ";
static const char str_spiffs[] PROGMEM = "spiffs";
static const char str_session_interrupted[] PROGMEM =
    "[cli] Your session was interrupted!";
static const char str_scanning[] PROGMEM = "scanning... ";
static const char str_switched[] PROGMEM = "switched ";
static const char str_ssid[] PROGMEM = "ssid ";
static const char str_start[] PROGMEM = "start ";
static const char str_stopped[] PROGMEM = "stopped ";
static const char str_store[] PROGMEM = "store ";
static const char str_sta[] PROGMEM = "sta ";
static const char str_set[] PROGMEM = "set ";
static const char str_system_time[] PROGMEM = "system time ";
static const char str_synced[] PROGMEM = "synced ";
static const char str_success[] PROGMEM = "success ";
static const char str_timezone[] PROGMEM = "timezone ";
static const char str_time[] PROGMEM = "time ";
static const char str_telnet[] PROGMEM = "[telnet] ";
static const char str_telnet_[] PROGMEM = "telnet";
static const char str_two_dots[] PROGMEM = ": ";    
static const char str_twp[] PROGMEM = "twp ";
static const char str_wifi[] PROGMEM = "[wifi] ";
static const char str_wifi_[] PROGMEM = "wifi ";
static const char str_wrong[] PROGMEM = "wrong ";
static const char str_yes[] PROGMEM = "yes ";

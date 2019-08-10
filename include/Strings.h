#pragma once

#include <pgmspace.h>

#include "Consts.h"

static const char stre_low_voltage[] PROGMEM = "low voltage ";

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
static const char strf_output_voltage[] PROGMEM = "Vout %.2f ";
static const char strf_filelist[] PROGMEM = "filelist %s";
static const char strf_power[] PROGMEM = "power %s ";
static const char strf_lu_sec[] PROGMEM = "for %lu sec ";
static const char strf_lu_ms[] PROGMEM = "for %lu ms ";
static const char strf_used_per[] PROGMEM = "used %.2f%%";
static const char strf_size_d[] PROGMEM = "size %d ";

static const char str_as_default[] PROGMEM = "as default ";
static const char str_ap[] PROGMEM = "ap ";
static const char str_arrow_dest[] PROGMEM = "-> ";
static const char str_avaible_system_actions[] PROGMEM =
    "Available actions for 'system' command are: 'reset', 'load', 'save', "
    "'restart'.";
static const char str_backup[] PROGMEM = "backup ";
static const char str_build_date[] PROGMEM = BUILD_DATE;
static const char str_capture[] PROGMEM = "capture ";
static const char str_clients[] = "clients";
static const char str_shell_start_hint[] PROGMEM = "[shell] press \"enter\" to start";
static const char str_clock[] PROGMEM = "[clock] ";
static const char str_config[] PROGMEM = "[config] ";
static const char str_connecting[] PROGMEM = "connecting ";
static const char str_connection[] PROGMEM = "connection ";
static const char str_connected[] PROGMEM = "connected ";
static const char str_complete[] PROGMEM = "complete ";
static const char str_date[] PROGMEM = "date ";
static const char str_disabled[] PROGMEM = "disabled ";
static const char str_disconnected[] PROGMEM = "disconnected ";
static const char str_dhcp_on[] PROGMEM = " dhcp on ";
static const char str_dns[] PROGMEM = "[dns] ";
static const char str_done[] PROGMEM = "done ";
static const char str_down[] PROGMEM = "down ";
static const char str_duration[] PROGMEM = "duration ";
static const char str_interval[] PROGMEM = "interval ";
static const char str_idle[] PROGMEM = "idle ";
static const char str_invalid[] PROGMEM = "invalid %s";
static const char str_http[] PROGMEM = "[http] ";
static const char str_http_[] PROGMEM = "http ";
static const char str_empty[] PROGMEM = "empty ";
static const char str_got[] PROGMEM = "got ";
static const char str_known[] PROGMEM = "known ";
static const char str_last[] PROGMEM = "last ";
static const char str_log[] PROGMEM = "log ";
static const char str_lcd[] PROGMEM = "[lcd] ";
static const char str_mdns[] PROGMEM = "[mdns] ";
static const char str_mode[] PROGMEM = "mode ";
static const char str_netbios[] PROGMEM = "[netbios] ";
static const char str_network[] PROGMEM = "network ";
static const char str_network_not_found[] PROGMEM = "no networks found";
static const char str_network_found[] PROGMEM = "%d networks found";
static const char str_no[] PROGMEM = "no ";
static const char str_ntp[] PROGMEM = "[ntp] ";
static const char str_elapsed[] PROGMEM = "elapsed ";
static const char str_error[] PROGMEM = "error ";
static const char str_failed[] PROGMEM = "failed ";
static const char str_firmware[] PROGMEM = "firmware";
static const char str_found[] PROGMEM = "found ";
static const char str_not_found[] PROGMEM = "not found ";
static const char str_ready[] PROGMEM = "ready";
static const char str_host[] PROGMEM = "host ";
static const char str_on[] PROGMEM = "on ";
static const char str_off[] PROGMEM = "off ";
static const char str_password[] PROGMEM = "password ";
static const char str_psu[] PROGMEM = "[psu] ";
static const char str_psu_log[] PROGMEM = "[psu-log] ";
static const char str_up[] PROGMEM = "up ";
static const char str_unknown[] PROGMEM = "unknown";
static const char str_update[] PROGMEM = "[update] ";
static const char str_unset[] PROGMEM = "<unset> ";
static const char str_unsecured[] PROGMEM = "<unsecured>";
static const char str_reconnect[] PROGMEM = "reconnect ";
static const char str_restore[] PROGMEM = "restore ";
static const char str_size[] PROGMEM = "size ";
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
static const char str_system_restart[] PROGMEM =
    "The system is going down for restart ";
static const char str_synced[] PROGMEM = "synced ";
static const char str_success[] PROGMEM = "success ";
static const char str_timezone[] PROGMEM = "timezone ";
static const char str_time[] PROGMEM = "time ";
static const char str_telnet[] PROGMEM = "[telnet] ";
static const char str_telnet_[] PROGMEM = "telnet ";
static const char str_two_dots[] PROGMEM = ": ";
static const char str_twp[] PROGMEM = "twp ";
static const char str_wifi[] PROGMEM = "[wifi] ";
static const char str_wifi_[] PROGMEM = "wifi ";
static const char str_wrong[] PROGMEM = "wrong ";
static const char str_wait[] PROGMEM = "wait";
static const char str_yes[] PROGMEM = "yes ";

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

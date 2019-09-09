#pragma once

#include <pgmspace.h>
#include "CommonTypes.h"
#include "Consts.h"

static const char HOST_NAME[] PROGMEM = "smartpower2";
static const char str_build_date[] PROGMEM = BUILD_DATE;

static const char str_shell_start_hint[] PROGMEM =
    "[shell] press \"enter\" to start";
static const char msg_system_restart[] PROGMEM =
    "the system is going down for restart!";
static const char msg_session_interrupted[] PROGMEM =
    "the session was interrupted!";
static const char msg_connection_is_busy[] PROGMEM = "connection is busy";

static const char strf_s_d[] PROGMEM = "%s:%d";
static const char strf_every_ms[] PROGMEM = "every %lu ms";
static const char strf_every_sec[] PROGMEM = "every %lu sec";
static const char strf_arrow_dest[] PROGMEM = "-> %s";
static const char strf_arrow_src[] PROGMEM = "<- %s";
static const char str_active[] PROGMEM = "active";
static const char strf_binnary[] PROGMEM = "binnary %s";
static const char strf_http_params[] PROGMEM = "%s %d(%d)";
static const char strf_wifi_params[] PROGMEM = "ssid %s ip %s";
static const char strf_wifi_scan_results[] PROGMEM = "#%d %s %d";
static const char strf_file_deleted[] PROGMEM = "file '%s' deleted";
static const char strf_file_not_found[] PROGMEM = "file '%s' not found";
static const char strf_config_param_value[] PROGMEM = "%s=\"%s\"";
static const char strf_synced[] PROGMEM = "synced %d";
static const char strf_timezone[] PROGMEM = "timezone %d";
static const char strf_mode[] PROGMEM = "mode %d";
static const char strf_host[] PROGMEM = "hostname %s";
static const char strf_tpw[] PROGMEM = "tpw %d";
static const char strf_ip[] PROGMEM = "ip %s";
static const char strf_in_second[] PROGMEM = "in %d seconds";
static const char strf_config_param_changed[] PROGMEM =
    "new value '%s' for the '%s' has been set!";
static const char strf_config_param_unchanged[] PROGMEM = "'%s' unchanged";
static const char strf_unknown_command_item[] PROGMEM =
    "unknown item '%s' for command '%s'";
static const char strf_unknown_action_param[] PROGMEM =
    "unknown param '%s' for action '%s'";
static const char strf_config_unknown_param[] PROGMEM = "Unknown param '%s'";
static const char strf_date[] PROGMEM = "%2d/%2d/%4d";
static const char strf_set_s[] PROGMEM = "set %s";
static const char strf_progress[] PROGMEM = " %u%%\r";
static const char strf_boot_progress[] PROGMEM = "%s %u%%";
static const char strf_client[] PROGMEM = "#%d";
static const char strf_set_broadcast[] PROGMEM = "set broadcast if%d to if%d";
static const char strf_ip_params[] PROGMEM =
    "ip %s subnet %s gateway %s dns %s";
static const char strf_mac[] PROGMEM = "mac %02x:%02x:%02x:%02x:%02x:%02x";
static const char strf_bssid[] PROGMEM = "bssid %02x:%02x:%02x:%02x:%02x:%02x";
static const char strf_channel[] PROGMEM = "ch %d";
static const char strf_ssid[] PROGMEM = "ssid %s";
static const char strf_heap[] PROGMEM = "heap %s";
static const char strf_passwd[] PROGMEM = "passwd %s";
static const char strf_ipaddr[] PROGMEM = "ipaddr %s";
static const char strf_ntp[] PROGMEM = "[ntp] %s";
static const char strf_interval[] PROGMEM = "interval %d";
static const char strf_filelist[] PROGMEM = "filelist %s";
static const char strf_power[] PROGMEM = "power %s";
static const char strf_f_sec[] PROGMEM = "%.2f sec";
static const char strf_lu_sec[] PROGMEM = "%lu sec";
static const char strf_lu_ms[] PROGMEM = "%lu ms";
static const char strf_used_per[] PROGMEM = "used %.2f%%";
static const char strf_size_d[] PROGMEM = "size %d";
static const char strf_time[] PROGMEM = "%02d:%02d:%02d";
static const char strf_per[] PROGMEM = "%.2f%%";

static const char str_action[] PROGMEM = "action";
static const char str_alert[] PROGMEM = "alert";
static const char str_as_default[] PROGMEM = "as default";
static const char str_ap[] PROGMEM = "ap";
static const char str_apply[] PROGMEM = "apply";
static const char str_arrow_dest[] PROGMEM = "->";
static const char str_arrow_src[] PROGMEM = "<-";
static const char str_avg[] PROGMEM = "avg";
static const char str_auth[] PROGMEM = "auth";
static const char str_backup[] PROGMEM = "backup";
static const char str_backlight[] PROGMEM = "backlight";
static const char str_begin[] PROGMEM = "begin";
static const char str_broadcast[] PROGMEM = "broadcast";
static const char str_btn[] PROGMEM = "btn";
static const char str_capture[] PROGMEM = "capture";
static const char str_capturing[] PROGMEM = "capturing";
static const char str_change[] PROGMEM = "change";
static const char str_clients[] = "clients";
static const char str_config[] PROGMEM = "config";
static const char str_complete[] PROGMEM = "complete";
static const char str_connect[] PROGMEM = "connect";
static const char str_connecting[] PROGMEM = "connecting";
static const char str_connection[] PROGMEM = "connection";
static const char str_connected[] PROGMEM = "connected";
static const char str_clock[] PROGMEM = "clock";
static const char str_close[] PROGMEM = "close";
static const char str_closed[] PROGMEM = "closed";
static const char str_cli[] PROGMEM = "cli";
static const char str_netsvc[] PROGMEM = "netsvc";
static const char str_data[] PROGMEM = "data";
static const char str_date[] PROGMEM = "date";
static const char str_disabled[] PROGMEM = "disabled";
static const char str_disconnected[] PROGMEM = "disconnected";
static const char str_dhcp[] PROGMEM = "dhcp";
static const char str_dns[] PROGMEM = "dns";
static const char str_done[] PROGMEM = "done";
static const char str_down[] PROGMEM = "down";
static const char str_duration[] PROGMEM = "duration";
static const char str_interval[] PROGMEM = "interval";
static const char str_info[] PROGMEM = "info";
static const char str_init[] PROGMEM = "init";
static const char str_idle[] PROGMEM = "idle";
static const char str_input[] PROGMEM = "input";
static const char str_ip[] PROGMEM = "ip";
static const char str_ms[] PROGMEM = "ms";
static const char str_read[] PROGMEM = "read";
static const char str_false[] PROGMEM = "false";
static const char str_free[] PROGMEM = "free";
static const char str_file[] PROGMEM = "file";
static const char str_invalid[] PROGMEM = "invalid";
static const char str_http[] PROGMEM = "http";
static const char str_empty[] PROGMEM = "empty";
static const char str_got[] PROGMEM = "got";
static const char str_known[] PROGMEM = "known";
static const char str_led[] PROGMEM = "led";
static const char str_load[] PROGMEM = "load";
static const char str_last[] PROGMEM = "last";
static const char str_left[] PROGMEM = "left";
static const char str_low_voltage[] PROGMEM = "low voltage";
static const char str_load_low[] PROGMEM = "load low";
static const char str_lcd[] PROGMEM = "lcd";
static const char str_min[] PROGMEM = "min";
static const char str_max[] PROGMEM = "max";
static const char str_mdns[] PROGMEM = "mdns";
static const char str_mode[] PROGMEM = "mode";
static const char str_name[] PROGMEM = "name";
static const char str_netbios[] PROGMEM = "netbios";
static const char str_network[] PROGMEM = "network";
static const char str_no[] PROGMEM = "no";
static const char str_ntp[] PROGMEM = "ntp";
static const char str_elapsed[] PROGMEM = "elapsed";
static const char str_end[] PROGMEM = "end";
static const char str_eof[] PROGMEM = "eof";
static const char str_event[] PROGMEM = "event";
static const char str_exist[] PROGMEM = "exist";
static const char str_none[] PROGMEM = "none";
static const char str_not[] PROGMEM = "not";
static const char str_error[] PROGMEM = "error";
static const char str_epoch[] PROGMEM = "epoch";
static const char str_log[] PROGMEM = "log";
static const char str_lps[] PROGMEM = "lps";
static const char str_failed[] PROGMEM = "failed";
static const char str_firmware[] PROGMEM = "firmware";
static const char str_found[] PROGMEM = "found";
static const char str_ready[] PROGMEM = "ready";
static const char str_host[] PROGMEM = "host";
static const char str_on[] PROGMEM = "on";
static const char str_ok[] PROGMEM = "ok";
static const char str_off[] PROGMEM = "off";
static const char str_open[] PROGMEM = "open";
static const char str_output[] PROGMEM = "output";
static const char str_over[] PROGMEM = "over";
static const char str_password[] PROGMEM = "password";
static const char str_param[] PROGMEM = "param";
static const char str_print[] PROGMEM = "print";
static const char str_reload[] PROGMEM = "reload";
static const char str_reset[] PROGMEM = "reset";
static const char str_psu[] PROGMEM = "psu";
static const char str_ping[] PROGMEM = "ping";
static const char str_pong[] PROGMEM = "pong";
static const char str_power[] PROGMEM = "power";
static const char str_rollover[] PROGMEM = "rollover";
static const char str_save[] PROGMEM = "save";
static const char str_status[] PROGMEM = "status";
static const char str_reconnect[] PROGMEM = "reconnect";
static const char str_reason[] PROGMEM = "reason";
static const char str_restart[] PROGMEM = "restart";
static const char str_restore[] PROGMEM = "restore";
static const char str_receive[] PROGMEM = "receive";
static const char str_samples[] PROGMEM = "samples";
static const char str_safe[] PROGMEM = "safe";
static const char str_sec[] PROGMEM = "sec";
static const char str_size[] PROGMEM = "size";
static const char str_spiffs[] PROGMEM = "spiffs";
static const char str_scanning[] PROGMEM = "scanning";
static const char str_switched[] PROGMEM = "switched";
static const char str_ssid[] PROGMEM = "ssid";
static const char str_start[] PROGMEM = "start";
static const char str_stopped[] PROGMEM = "stopped";
static const char str_store[] PROGMEM = "store";
static const char str_sta[] PROGMEM = "sta";
static const char str_set[] PROGMEM = "set";
static const char str_system[] PROGMEM = "system";
static const char str_redirected[] PROGMEM = "redirected";
static const char str_state[] PROGMEM = "state";
static const char str_shell[] PROGMEM = "shell";
static const char str_synced[] PROGMEM = "synced";
static const char str_success[] PROGMEM = "success";
static const char str_task[] PROGMEM = "task";
static const char str_tcp[] PROGMEM = "tcp";
static const char str_timeout[] PROGMEM = "timeout";
static const char str_timezone[] PROGMEM = "timezone";
static const char str_time[] PROGMEM = "time";
static const char str_true[] PROGMEM = "true";
static const char str_trusted[] PROGMEM = "trusted";
static const char str_total[] PROGMEM = "total";
static const char str_telnet[] PROGMEM = "telnet";
static const char str_tpw[] PROGMEM = "tpw";
static const char str_two_dots[] PROGMEM = ":";
static const char str_up[] PROGMEM = "up";
static const char str_unknown[] PROGMEM = "unknown";
static const char str_unhandled[] PROGMEM = "unhandled";
static const char str_unset[] PROGMEM = "unset";
static const char str_unsecured[] PROGMEM = "unsecured";
static const char str_update[] PROGMEM = "update";
static const char str_upload[] PROGMEM = "upload";
static const char str_uptime[] PROGMEM = "uptime";
static const char str_used[] PROGMEM = "used";
static const char str_valid[] PROGMEM = "valid";
static const char str_voltage[] PROGMEM = "voltage";
static const char str_wifi[] PROGMEM = "wifi";
static const char str_wrong[] PROGMEM = "wrong";
static const char str_wait[] PROGMEM = "wait";
static const char str_write[] PROGMEM = "write";
static const char str_yes[] PROGMEM = "yes";

static PGM_P module_name[] PROGMEM = {
    str_btn,  str_clock,  str_led, str_psu,    str_task,  str_shell, str_lcd,
    str_http, str_netsvc, str_ntp, str_telnet, str_shell, str_update};

inline String getIdentStr(const char* str, bool with_space = true,
                          char left = '[', char right = ']') {
    char buf[64];
    memset(buf, 0, 64);
    buf[0] = left;
    uint8_t x = strlen(strcat(buf, str));
    buf[x] = right;
    if (with_space) buf[++x] = ' ';
    buf[++x] = '\x00';
    return String(buf);
}

inline String getQuotedStr(const char* str, bool with_space = true,
                           char ch = '\'') {
    return getIdentStr(str, with_space, ch, ch);
}

inline String getQuotedStr(String& str, bool space = true) {
    return "'" + str + (space ? "' " : "'");
}

inline String getIdentStrP(PGM_P strP, bool with_space = true) {
    char buf[64];
    memset(buf, 0, 64);
    strcpy_P(buf, strP);
    return getIdentStr(buf, with_space);
}

inline String getStrP(PGM_P strP, bool space = true) {
    char buf[64];
    memset(buf, 0, 64);
    strcpy_P(buf, strP);
    if (space) {
        size_t size = strlen(buf);
        buf[size] = '\x20';
        buf[++size] = '\x00';
    }
    return String(buf);
}

inline String getBoolStr(bool value, bool space = true) {
    return value ? getStrP(str_true, space) : getStrP(str_false, space);
}

inline String getStr(const char* str) {
    String res(str);
    return res + " ";
}

inline String getStr(int num) {
    String res(num, DEC);
    return res + " ";
}

inline String getStr(const String& str) { return str + " "; }

inline String getModuleName(uint8_t index) {
    return getStrP((char*)pgm_read_ptr(&(module_name[index])), true);
}

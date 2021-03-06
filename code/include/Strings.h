#pragma once

#include <pgmspace.h>

// TODO
// now it's not for memory saving, but to collect literals in one place
static const char strf_json_serilialize[] PROGMEM = "json serialize (%s)";
static const char strf_invalid_param[] PROGMEM = "invalid param (%s)";
static const char str_dns_resolve[] PROGMEM = "dsn resolve";
static const char str_interrupted[] PROGMEM = "interrupted";
static const char str_down[] PROGMEM = "down";
static const char str_up[] PROGMEM = "up";

static const char str_add[] PROGMEM = "add";
static const char str_bootpwr[] PROGMEM = "bootpwr";
static const char str_passwd[] PROGMEM = "passwd";
static const char str_ap_ssid[] PROGMEM = "ap_ssid";
static const char str_ap_passwd[] PROGMEM = "ap_passwd";    
static const char str_ap_ipaddr[] PROGMEM = "ap_ipaddr";
static const char str_time_zone[] PROGMEM = "time_zone";
static const char str_twp[] PROGMEM =  "twp";
static const char str_ntp_sync[] PROGMEM =  "ntp_sync";
static const char str_ntp_pool[] PROGMEM =  "ntp_pool";
static const char str_time_backup[] PROGMEM = "time_backup";
static const char str_store_wh[] PROGMEM =  "store_wh";

static const char str_exit_bye[] PROGMEM = "exit, bye!";
static const char str_not_found[] PROGMEM = "not found";
static const char str_not_exist[] PROGMEM = "not exist";
static const char msg_greetings[] PROGMEM = "Welcome! To exit press ESC twice.";
static const char msg_restart[] PROGMEM = "System restart!";

static const char strf_ip[] PROGMEM = "ip %s";
static const char strf_config_param_unchanged[] PROGMEM = "'%s' unchanged";
static const char strf_progress[] PROGMEM = " %u%%\r";

static const char strf_ssid[] PROGMEM = "ssid %s";
static const char strf_heap[] PROGMEM = "heap %s";
static const char strf_passwd[] PROGMEM = "passwd %s";
static const char strf_ipaddr[] PROGMEM = "ipaddr %s";
static const char strf_interval[] PROGMEM = "interval %d";
static const char strf_power[] PROGMEM = "power %s";
static const char strf_f_sec[] PROGMEM = "%.2f sec";
static const char strf_lu_sec[] PROGMEM = "%lu sec";
static const char strf_lu_ms[] PROGMEM = "%lu ms";
static const char strf_used_per[] PROGMEM = "used %.2f%%";
static const char strf_size_d[] PROGMEM = "size %d";
static const char strf_per[] PROGMEM = "%.2f%%";

static const char str_app[] PROGMEM = "app";
static const char str_ap_sta[] PROGMEM = "ap sta";
static const char str_active[] PROGMEM = "active";
static const char str_action[] PROGMEM = "action";
static const char str_alert[] PROGMEM = "alert";
static const char str_as_default[] PROGMEM = "as default";
static const char str_ap[] PROGMEM = "ap";
static const char str_ap_not_found[] PROGMEM = "ap not found";
static const char str_apply[] PROGMEM = "apply";
static const char str_arrow_dest[] PROGMEM = "->";
static const char str_arrow_src[] PROGMEM = "<-";
static const char str_avg[] PROGMEM = "avg";
static const char str_auth[] PROGMEM = "auth";
static const char str_backup[] PROGMEM = "backup";
static const char str_backlight[] PROGMEM = "backlight";
static const char str_begin[] PROGMEM = "begin";
static const char str_bssid[] PROGMEM = "bssid";
static const char str_broadcast[] PROGMEM = "broadcast";
static const char str_btn[] PROGMEM = "btn";
static const char str_bind[] PROGMEM = "bind";
static const char str_boot[] PROGMEM = "boot";
static const char str_capture[] PROGMEM = "capture";
static const char str_crash[] PROGMEM = "crash";
static const char str_capturing[] PROGMEM = "capturing";
static const char str_changed[] PROGMEM = "changed";
static const char str_station[] PROGMEM = "station";
static const char str_ch[] PROGMEM = "ch";
static const char str_chip[] PROGMEM = "chip";
static const char str_change[] PROGMEM = "change";
static const char str_clear[] = "clear";
static const char str_client[] = "client";
static const char str_clients[] = "clients";
static const char str_config[] PROGMEM = "config";
static const char str_count[] PROGMEM = "count";
static const char str_complete[] PROGMEM = "complete";
static const char str_connect[] PROGMEM = "connect";
static const char str_connecting[] PROGMEM = "connecting";
static const char str_connection[] PROGMEM = "connection";
static const char str_connected[] PROGMEM = "connected";
static const char str_console[] PROGMEM = "console";
static const char str_clock[] PROGMEM = "clock";
static const char str_close[] PROGMEM = "close";
static const char str_closed[] PROGMEM = "closed";
static const char str_cli[] PROGMEM = "cli";
static const char str_control[] PROGMEM = "control";
static const char str_cpu[] PROGMEM = "cpu";
static const char str_core[] PROGMEM = "core";
static const char str_connection_failed[] PROGMEM = "connection failed";
static const char str_netsvc[] PROGMEM = "netsvc";
static const char str_daylight[] PROGMEM = "daylight";
static const char str_data[] PROGMEM = "data";
static const char str_date[] PROGMEM = "date";
static const char str_debug[] PROGMEM = "debug";
static const char str_default[] PROGMEM = "default";
static const char str_deleted[] PROGMEM = "deleted";
static const char str_disabled[] PROGMEM = "disabled";
static const char str_display[] PROGMEM = "display";
static const char str_disconnected[] PROGMEM = "disconnected";
static const char str_dhcp[] PROGMEM = "dhcp";
static const char str_dns[] PROGMEM = "dns";
static const char str_dir[] PROGMEM = "dir";
static const char str_done[] PROGMEM = "done";
static const char str_domain[] PROGMEM = "domain";
static const char str_diff[] PROGMEM = "diff";
static const char str_delete[] PROGMEM = "delete";
static const char str_duration[] PROGMEM = "duration";
static const char str_interval[] PROGMEM = "interval";
static const char str_enabled[] PROGMEM = "enabled";
static const char str_every[] PROGMEM = "every";
static const char str_info[] PROGMEM = "info";
static const char str_id[] PROGMEM = "id";
static const char str_init[] PROGMEM = "init";
static const char str_idle[] PROGMEM = "idle";
static const char str_input[] PROGMEM = "input";
static const char str_ip[] PROGMEM = "ip";
static const char str_ipaddr[] PROGMEM = "ipaddr";
static const char str_item[] PROGMEM = "item";
static const char str_false[] PROGMEM = "false";
static const char str_free[] PROGMEM = "free";
static const char str_freq[] PROGMEM = "freq";
static const char str_full[] PROGMEM = "full";
static const char str_file[] PROGMEM = "file";
static const char str_for[] PROGMEM = "for";
static const char str_format[] PROGMEM = "format";
static const char str_fw[] PROGMEM = "fw";
static const char str_sdk[] PROGMEM = "sdk";
static const char str_invalid[] PROGMEM = "invalid";
static const char str_invalid_state[] PROGMEM = "invalid state";
static const char str_home[] PROGMEM = "home";
static const char str_heap[] PROGMEM = "heap";
static const char str_http[] PROGMEM = "http";
static const char str_empty[] PROGMEM = "empty";
static const char str_gateway[] PROGMEM = "gateway";
static const char str_got[] PROGMEM = "got";
static const char str_got_ip[] PROGMEM = "got ip";
static const char str_known[] PROGMEM = "known";
static const char str_led[] PROGMEM = "led";
static const char str_load[] PROGMEM = "load";
static const char str_last[] PROGMEM = "last";
static const char str_left[] PROGMEM = "left";
static const char str_list[] PROGMEM = "list";
static const char str_local[] PROGMEM = "local";
static const char str_low_voltage[] PROGMEM = "low voltage";
static const char str_load_low[] PROGMEM = "load low";
static const char str_lcd[] PROGMEM = "lcd";
static const char str_mac[] PROGMEM = "mac";
static const char str_max[] PROGMEM = "max";
static const char str_mod[] PROGMEM = "mod";
static const char str_mdns[] PROGMEM = "mdns";
static const char str_min[] PROGMEM = "min";
static const char str_mode[] PROGMEM = "mode";
static const char str_module[] PROGMEM = "module";
static const char str_ms[] PROGMEM = "ms";
static const char str_name[] PROGMEM = "name";
static const char str_netmask[] PROGMEM = "netmask";
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
static const char str_error[] PROGMEM = "error";
static const char str_epoch[] PROGMEM = "epoch";
static const char str_login[] PROGMEM = "login";
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
static const char str_offset[] PROGMEM ="offset";
static const char str_open[] PROGMEM = "open";
static const char str_output[] PROGMEM = "output";
static const char str_over[] PROGMEM = "over";
static const char str_other[] PROGMEM = "other";
static const char str_password[] PROGMEM = "password";
static const char str_param[] PROGMEM = "param";
static const char str_path[] PROGMEM = "path";
static const char str_port[] PROGMEM = "port";
static const char str_print[] PROGMEM = "print";
static const char str_progress[] PROGMEM = "progress";
static const char str_reload[] PROGMEM = "reload";
static const char str_reset[] PROGMEM = "reset";
static const char str_psu[] PROGMEM = "psu";
static const char str_psu_log[] PROGMEM = "log";
static const char str_page[] PROGMEM = "page";
static const char str_ping[] PROGMEM = "ping";
static const char str_pong[] PROGMEM = "pong";
static const char str_power[] PROGMEM = "power";
static const char str_phy[] PROGMEM = "phy";
static const char str_read[] PROGMEM = "read";
static const char str_rssi[] PROGMEM = "rssi";
static const char str_request[] PROGMEM = "request";
static const char str_response[] PROGMEM = "response";
static const char str_reconnect[] PROGMEM = "reconnect";
static const char str_reason[] PROGMEM = "reason";
static const char str_restart[] PROGMEM = "restart";
static const char str_restore[] PROGMEM = "restore";
static const char str_restored[] PROGMEM = "restored";
static const char str_resolve[] PROGMEM = "resolve";
static const char str_receive[] PROGMEM = "receive";
static const char str_redirected[] PROGMEM = "redirected";
static const char str_rollover[] PROGMEM = "rollover";
static const char str_samples[] PROGMEM = "samples";
static const char str_safe[] PROGMEM = "safe";
static const char str_save[] PROGMEM = "save";
static const char str_scanning[] PROGMEM = "scanning";
static const char str_show[] PROGMEM = "show";
static const char str_sec[] PROGMEM = "sec";
static const char str_server[] PROGMEM = "server";
static const char str_size[] PROGMEM = "size";
static const char str_littlefs[] PROGMEM = "littlefs";
static const char str_status[] PROGMEM = "status";
static const char str_switched_on[] PROGMEM = "switched on";
static const char str_switched_off[] PROGMEM = "switched off";
static const char str_ssid[] PROGMEM = "ssid";
static const char str_start[] PROGMEM = "start";
static const char str_stop[] PROGMEM = "stop";
static const char str_stopped[] PROGMEM = "stopped";
static const char str_store[] PROGMEM = "store";
static const char str_sta[] PROGMEM = "sta";
static const char str_set[] PROGMEM = "set";
static const char str_run[] PROGMEM = "run";
static const char str_state[] PROGMEM = "state";
static const char str_stored[] PROGMEM = "stored";
static const char str_shell[] PROGMEM = "shell";
static const char str_switch[] PROGMEM = "switch";
static const char str_success[] PROGMEM = "success";
static const char str_subnet[] PROGMEM = "subnet";
static const char str_synced[] PROGMEM = "synced";
static const char str_system[] PROGMEM = "system";
static const char str_syslog[] PROGMEM = "syslog";
static const char str_shift[] PROGMEM = "shift";
static const char str_test[] PROGMEM = "test";
static const char str_task[] PROGMEM = "task";
static const char str_tcp[] PROGMEM = "tcp";
static const char str_toggle[] PROGMEM = "toggle";
static const char str_timeout[] PROGMEM = "timeout";
static const char str_timezone[] PROGMEM = "timezone";
static const char str_time[] PROGMEM = "time";
static const char str_true[] PROGMEM = "true";
static const char str_trusted[] PROGMEM = "trusted";
static const char str_total[] PROGMEM = "total";
static const char str_telnet[] PROGMEM = "telnet";
static const char str_tpw[] PROGMEM = "tpw";
static const char str_unknown[] PROGMEM = "unknown";
static const char str_unhandled[] PROGMEM = "unhandled";
static const char str_unset[] PROGMEM = "unset";
static const char str_unsecured[] PROGMEM = "unsecured";
static const char str_update[] PROGMEM = "update";
static const char str_updated[] PROGMEM = "updated";
static const char str_upload[] PROGMEM = "upload";
static const char str_uptime[] PROGMEM = "uptime";
static const char str_unsupported[] PROGMEM = "unsupported";
static const char str_utc[] PROGMEM = "utc";
static const char str_used[] PROGMEM = "used";
static const char str_valid[] PROGMEM = "valid";
static const char str_version[] PROGMEM = "version";
static const char str_voltage[] PROGMEM = "voltage";
static const char str_wifi[] PROGMEM = "wifi";
static const char str_probe[] PROGMEM = "probe";
static const char str_wifi_ap[] PROGMEM = "wifi ap";
static const char str_wifi_ap_sta[] PROGMEM = "wifi ap sta";
static const char str_wifi_sta[] PROGMEM = "wifi sta";
static const char str_value[] PROGMEM = "value";
static const char str_wait[] PROGMEM = "wait";
static const char str_warn[] PROGMEM = "warn";
static const char str_write[] PROGMEM = "write";
static const char str_wrong[] PROGMEM = "wrong";
static const char str_wrong_password[] PROGMEM = "wrong password";
static const char str_wh[] PROGMEM = "wh";
static const char str_wol[] PROGMEM = "wol";
static const char str_yes[] PROGMEM = "yes";
static const char str_web[] PROGMEM = "web";
static const char str_busy[] PROGMEM = "busy";
static const char str_wire[] PROGMEM = "wire";

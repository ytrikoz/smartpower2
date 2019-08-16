#pragma once

#include "BuildConfig.h"

#define BOOT_WAIT_s 5
#define BUILD_TIMEZONE_h 3
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__
#define APPNAME "SmartPower2"
#define FW_VERSION "0.6.5"            
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

#define MEASUREMENT_INTERVAL_ms 250
#define START_SHOWING_RESTART_COUNTER_ON 5
#define LOOP_STATS_INTERVAL 5000

#define FILENAME_MAX_LENGTH 31
#define FILE_CONFIG "/cfg/settings.ini"
#define FILE_LAST_POWER_STATE "/cfg/powerstate"
#define FILE_TIME_BACKUP "/cfg/clock"
#define FILE_WEB_SETTINGS "/www/js/settings.js"

#define PARAM_COUNT 20
#define PARAM_NAME_STR_SIZE 12
#define PARAM_BOOL_SIZE 2
#define PARAM_STR_SIZE 33
#define PARAM_IPADDR_SIZE 17
#define PARAM_OUTPUT_VOLTAGE_SIZE 8
#define PARAM_NUMBER_SIZE 4
#define PARAM_LARGE_NUMBER_SIZE 9

#define HOST_NAME "smartpower2"
#define HOST_DOMAIN "local"
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

#define PSU_CHECK_INTERVAL_s 5
#define PSU_VOLTAGE_LOW_v 1.0f
#define PSU_LOAD_LOW_a 0.001f
#define PSU_LOG_SIZE 128
#define PSU_LOG_INTERVAL_ms 250

#define DISPLAY_VIRTUAL_COLS 63
#define DISPLAY_VIRTUAL_ROWS 4

#define SCREEN_WIDTH 80
#define INPUT_MAX_LENGTH 128
#define OUTPUT_MAX_LENGTH 128

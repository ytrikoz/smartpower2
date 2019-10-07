#pragma once

#include "BuildConfig.h"

#define PG_HOME 1
#define PG_SETTINGS 2
#define PG_STATUS 3

#define SET_POWER_ON_OFF 'o'
#define SET_DEFAULT_VOLTAGE 'v'
#define SET_VOLTAGE 'w'
#define SET_BOOT_POWER_MODE 'a'
#define SET_NETWORK 'n'
#define SET_LOG_WATTHOURS 'm'

#define GET_PAGE_STATE 'p'

#define TAG_PVI 'd'
#define TAG_FIRMWARE_INFO 'f'
#define TAG_SYSTEM_INFO 'S'
#define TAG_NETWORK_INFO 'N'

#define BOOT_WAIT_s 5
#define BUILD_TIMEZONE_h 3
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

#define ONE_MILLISECOND_mi 1000
#define ONE_SECOND_ms 1000
#define ONE_MINUTE_s 60
#define ONE_MINUTE_ms ONE_MINUTE_s *ONE_SECOND_ms
#define ONE_DAY_s 86400L
#define ONE_WEEK_days 7
#define ONE_HOUR_s 3600
#define ONE_YEAR_days 365

#define ONE_HOUR_m 60
#define ONE_DAY_h 24
#define ONE_HOUR_ms ONE_HOUR_s *ONE_SECOND_ms;
#define ONE_WATT_mW 1000
#define SEVENTY_YEARS_ms 2208988800UL
#define MILLENIUM_s 946684800UL

#define START_YEAR 1970

#define MEASUREMENT_INTERVAL_ms 250
#define START_SHOWING_RESTART_COUNTER_ON 5
#define LOOP_STATS_INTERVAL 5000

#define FILENAME_SIZE 31

#define DIR_PSU_LOG "/log/"

#define FILE_WEB_SETTINGS "/www/js/settings.js"
#define FILE_CONFIG "/etc/main"
#define FILE_VAR_POWER_STATE "/var/ps"
#define FILE_VAR_UTC "/var/utc"
#define FILE_VAR_WH "/var/wh"

#define CONFIG_CHAR 1
#define CONFIG_STR 31
#define CONFIG_IPADDR 16
#define CONFIG_FLOAT 8
#define CONFIG_BYTE 4
#define CONFIG_NUMBER 9

#define HOST_DOMAIN "local"
#define WIFI_CONNECT_TIMEOUT 40
#define MAX_WEB_CLIENTS 5
#define OTA_PORT 8266
#define DNS_PORT 53
#define HTTP_WEB_ROOT "/www"
#define HTTP_PORT 80
#define WEBSOCKET_PORT 81
#define TELNET_PORT 23

#define NTP_REMOTE_PORT 123
#define NTP_LOCAL_PORT 2390

#define DEF_NTP_POOL_SERVER "pool.ntp.org"
#define DEF_TIME_BACKUP_INTERVAL_s "3600"
#define TIME_BACKUP_INTERVAL_MIN_s 60
#define WOL_PORT 9

#define PSU_CHECK_INTERVAL_s 5
#define PSU_VOLTAGE_LOW_v 1.0f
#define PSU_LOAD_LOW_a 0.001f
#define PSU_LOG_INTERVAL_ms 250
#define PSU_LOG_VOLTAGE_SIZE 128
#define PSU_LOG_CURRENT_SIZE 128
#define PSU_LOG_POWER_SIZE 128
#define PSU_LOG_WATTHOURS_SIZE 128

#define SCREEN_WIDTH 80
#define INPUT_MAX_LENGTH 128
#define OUTPUT_MAX_LENGTH 128

#define SHELL_HISTORY_SIZE 4
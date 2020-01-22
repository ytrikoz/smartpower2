#pragma once

#include "BuildConfig.h"

#define CRASH_SIZE 32 + 512 + 1
#define CRASH_NUM 10

#define TAG_PAGE_REQUEST 'p'

#define TAG_MAIN_PAGE "M"
#define TAG_OPTIONS_PAGE "O"
#define TAG_INFO_PAGE 'I'

#define TAG_SET_ON_OFF 'o'
#define TAG_SET_STORE_WH_TOTAL 'm'
#define TAG_SET_VOLTAGE 'w'
#define TAG_SET_NETWORK 'n'

#define SET_DEFAULT_VOLTAGE 'v'

#define SET_BOOT_POWER_MODE 'a'

#define TAG_PVI 'd'
#define TAG_FIRMWARE_INFO 'f'

#define TAG_NETWORK_INFO 'N'

#define BOOT_WAIT_s 5
#define BUILD_TIMEOFFSET_s 3 * 60
#define BUILD_DATE __DATE__
#define BUILD_TIME __TIME__

#define ONE_MILLISECOND_mi 1000
#define ONE_SECOND_ms 1000
#define ONE_MINUTE_s 60
#define ONE_MINUTE_ms ONE_MINUTE_s* ONE_SECOND_ms
#define ONE_HOUR_m 60
#define ONE_DAY_s 86400L
#define ONE_WEEK_days 7
#define ONE_HOUR_s ONE_HOUR_m* ONE_MINUTE_s
#define ONE_YEAR_days 365

#define ONE_DAY_h 24
#define ONE_HOUR_ms ONE_HOUR_s* ONE_SECOND_ms
#define ONE_WATT_mW 1000.0
#define SEVENTY_YEARS_ms 2208988800UL
#define MILLENIUM_s 946684800UL

#define START_YEAR 1970

#define MEASUREMENT_INTERVAL_ms 250
#define START_SHOWING_RESTART_COUNTER_ON 5
#define LOOP_STATS_INTERVAL 5000

#define FILENAME_SIZE 31

#define FS_CONFIG_ROOT "/etc/"
#define FS_LOG_ROOT "/log/"
#define FS_WEB_ROOT "/www/"
#define CRASH_ROOT "/err/"
#define FS_VAR_ROOT "/var/"
#define BOOT_FLAG "/var/boot"

#define FS_VERSION FS_WEB_ROOT "version"
#define FS_SYSTEM FS_WEB_ROOT "system"

#define FS_MAIN_CONFIG FS_CONFIG_ROOT "main"
#define FS_POWER_STATE_VAR FS_VAR_ROOT "ps"
#define FS_UTC_VAR FS_VAR_ROOT "utc"
#define FS_WH_VAR FS_VAR_ROOT "wh"
#define FS_HOST_VAR FS_VAR_ROOT "host"

#define HOST_DOMAIN "local"
#define WIFI_CONNECT_TIMEOUT 40
#define WEB_SERVER_CLIENT_MAX 5
#define OTA_PORT 8266
#define DNS_PORT 53
#define HTTP_PORT 80
#define WEBSOCKET_PORT 81
#define TELNET_PORT 23

#define NTP_REMOTE_PORT 123
#define NTP_LOCAL_PORT 2390
#define NTP_RETRY_INTERVAL 64
#define NTP_TIMEOUT 1000

#define DEF_NTP_POOL_SERVER "pool.ntp.org"
#define DEF_TIME_BACKUP_INTERVAL_s "3600"
#define TIME_BACKUP_INTERVAL_MIN_s 60
#define WOL_PORT 9

#define SYSLOG_PORT 514

#define PSU_CHECK_INTERVAL_s 5
#define PSU_VOLTAGE_LOW_v 1.0f
#define PSU_LOAD_LOW_a 0.001f
#define PSU_LOG_INTERVAL_ms 250
#define PSU_LOG_VOLTAGE_SIZE 64
#define PSU_LOG_CURRENT_SIZE 64
#define PSU_LOG_POWER_SIZE 64
#define PSU_LOG_WATTHOURS_SIZE 64

#define SCREEN_WIDTH 80
#define INPUT_MAX_LENGTH 128
#define OUTPUT_MAX_LENGTH 128

#define SHELL_HISTORY_SIZE 4

#define ONE_MHz_hz 1000000UL
#define HOLD_TIME_RESTART_s 5
#define HOLD_TIME_RESET_s 10

static const char TIME_FORMAT[] PROGMEM = "%02d:%02d:%02d";
static const char TIME_LONG_FORMAT[] PROGMEM = "%s %02d:%02d:%02d";
static const char DATETIME_FORMAT[] PROGMEM = "%02d/%02d/%04d %02d:%02d:%02d";
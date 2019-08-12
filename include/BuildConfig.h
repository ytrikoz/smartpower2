#pragma once

#include <HardwareSerial.h>

#define DEBUG Serial
#define USE_SERIAL Serial

#define NO_GLOBAL_SSDP
#define NO_GLOBAL_MDNS
#define NO_GLOBAL_NBNS
#define NO_GLOBAL_NETBIOS
#define NO_GLOBAL_ARDUINOOTA

//#define SERIAL_DEBUG

//#define DEBUG_LEDS
//#define DEBUG_LOOP
#define DEBUG_PLOT
//#define DEBUG_DISPLAY
//#define DEBUG_CONFIG
//#define DEBUG_SYSTEM_CLOCK
//#define DEBUG_WIRELESS
//#define DEBUG_NTP
//#define DEBUG_TELNET
//#define DEBUG_WEBSOCKET
#define DEBUG_HTTP
//#define DEBUG_SHELL
//#define DEBUG_TERMUL
//#define DEBUG_FILE_STORAGE
//#define DEBUG_TIME_BACKUP
//#define DEBUG_TIME_UTILS

//#define DISABLE_HTTP
//#define DISABLE_LCD
//#define DISABLE_TIME_BACKUP
//#define DISABLE_TELNET
//#define DISABLE_TELNET_SHELL
//#define DISABLE_CONSOLE_SHELL
//#define DISABLE_OTA_UPDATE
//#define DISABLE_NTP
//#define DISABLE_NETWORK_DISCOVERY
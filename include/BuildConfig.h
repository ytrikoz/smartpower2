#pragma once

#include <HardwareSerial.h>

#define I2C_DELAY 10
#define I2C_SDA D2
#define I2C_SCL D5
#define POWER_LED_PIN D1
#define WIFI_LED_PIN D4
#define POWER_BTN_PIN D7
#define INA231_ALERT_PIN 16
#define INA231_I2C_ADDR 0x40

#define DEBUG Serial
#define ERROR Serial
#define USE_SERIAL Serial

#define NO_GLOBAL_SSDP
#define NO_GLOBAL_MDNS
#define NO_GLOBAL_NBNS
#define NO_GLOBAL_NETBIOS
#define NO_GLOBAL_ARDUINOOTA

//#define SERIAL_DEBUG

//#define DEBUG_APP_MOD
//#define DEBUG_LEDS
//#define DEBUG_LOOP
//#define DEBUG_PLOT
//#define DEBUG_DISPLAY
//#define DEBUG_CONFIG
//#define DEBUG_FILE_STORAGE
//#define DEBUG_SYSTEM_CLOCK
//#define DEBUG_WIRELESS
//#define DEBUG_NTP
//#define DEBUG_TELNET
//#define DEBUG_WEB_SERVICE
//#define DEBUG_SHELL
#define DEBUG_TERMUL

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

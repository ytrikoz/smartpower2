#pragma once

#include <HardwareSerial.h>

// ADC_MODE(ADC_VCC);

#define APP_NAME "SmartPower2"
#define APP_SHORT "SP2"
#define APP_VERSION "0.6.9"
#define APP_BUILD_COMMIT ""


// address are 0x27 or 0x3f
#define NONE_ADDRESS 0x00
#define LCD_SLAVE_ADDRESS 0x3f
#define LCD_SLAVE_ADDRESS_ALT 0x27
#define LCD_SCROLL_INTERVAL 5000
#define LCD_UPDATE_INTERVAL 250
#define LCD_COLS 16
#define LCD_ROWS 2
#define LCD_ROW_1 0
#define LCD_ROW_2 1

#define I2C_DELAY 10
#define I2C_SDA D2
#define I2C_SCL D5

#define POWER_BTN_PIN D7
#define INA231_ALERT_PIN 16
#define INA231_I2C_ADDR 0x40

#define DEBUG Serial
#define ERROR Serial
#define INFO Serial

#define NO_GLOBAL_SSDP
#define NO_GLOBAL_MDNS
#define NO_GLOBAL_NBNS
#define NO_GLOBAL_NETBIOS
#define NO_GLOBAL_ARDUINOOTA
#define NO_GLOBAL_EEPROM
#define ATOMIC_FS_UPDATE

//#define INFO_DEBUG

//#define DEBUG_APP
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

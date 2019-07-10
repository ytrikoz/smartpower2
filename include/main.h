//#include <GDBStub.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "HardwareSerial.h"

#include "mcp4652.h"

#include "global.h"
#include "ip_utils.h"
#include "str_utils.h"

#include "Consts.h"
#include "Led.h"
#include "SysInfo.h"

#define I2C_SDA D2
#define I2C_SCL D5
#define POWER_LED_PIN D1
#define WIFI_LED_PIN D4
#define POWER_BTN_PIN D7
#define POWER_SWITCH_PIN D6

#define PAGE_HOME 1
#define PAGE_SETTINGS 2
#define PAGE_STATUS 3

#define POWER_ONOFF 'o'
#define SET_DEFAULT_VOLTAGE 'v'
#define SET_VOLTAGE 'w'
#define SAVE_NETWORKS 'n'
#define SET_POWERMODE 'a'
#define PAGE_STATE 'p'
#define DATA_PVI 'd'
#define MEASURE_MODE 'm'

#define TAG_FIRMWARE_INFO 'f'
#define TAG_SYSTEM_INFO 'S'
#define TAG_NETWORK_INFO 'N'

extern PowerState get_state();
extern void set_power_state(PowerState state);
extern ulong get_lps();
extern ulong get_longest_loop();

static void ICACHE_RAM_ATTR powerButtonHandler();

void setup(void);
void loop(void);
void timerHandler();
void setup_hardware();

void onClient(uint8_t, bool);
void onClientData(uint8_t num, String data);
void sendClients(String, uint8_t);
void sendClients(String, uint8_t, uint8_t);
void sendOnPageState(uint8_t num, uint8_t page);

bool formatSPIFFS();
void systemRestart(uint8_t sec = 0);

void set_power_switch(PowerState);
void set_output_voltage(float value);

void update_display();
void update_wifi_led();

void delaySequence(uint8_t sec);
void start_wifi();
volatile bool network = false;

ADC_MODE(ADC_VCC);

#ifndef DISABLE_HTTP
WebService *http;
#endif

#ifndef DISABLE_DISPLAY
Display *display;
#endif

#ifndef DISABLE_OTA
OTAUpdate *ota;
#endif

String getSystemStatus();

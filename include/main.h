//#include <GDBStub.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "HardwareSerial.h"
#include "Types.h"
#include "mcp4652.h"

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
#define SET_MEASURE_MODE 'm'

#define TAG_FIRMWARE_INFO 'f'
#define TAG_SYSTEM_INFO 'S'
#define TAG_NETWORK_INFO 'N'

extern void onHttpClientConnect(uint8_t);
extern void onHttpClientDisconnect(uint8_t);
extern void onHttpClientData(uint8_t num, String data);

extern PowerState get_state();
extern void set_power_state(PowerState state);
extern ulong get_lps();
extern ulong get_longest_loop();

static void ICACHE_RAM_ATTR powerButtonHandler();

void setup(void);
void loop(void);
void timerHandler();
void setup_hardware();
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
ADC_MODE(ADC_VCC);
int quadraticRegression(double volt);
void onBootProgress(uint8_t per, const char *message = NULL);

#include <Arduino.h>

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

void onHttpClientConnect(uint8_t);
void onHttpClientDisconnect(uint8_t);
void onHttpClientData(uint8_t n, String data);

void sendToClients(String, uint8_t);
void sendToClients(String, uint8_t, uint8_t);

void sendPageState(uint8_t n, uint8_t page);

void delay_print(Print* p);
void display_boot_progress(uint8_t per, const char* text = NULL);

void power_button_handler();
static void ICACHE_RAM_ATTR power_button_state_change();

// ADC_MODE(ADC_VCC);

void setup();
void loop();
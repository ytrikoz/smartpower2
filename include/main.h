#include <Arduino.h>


void onHttpClientConnect(uint8_t);
void onHttpClientDisconnect(uint8_t);
void onHttpClientData(uint8_t n, String data);

void sendToClients(String, uint8_t);
void sendToClients(String, uint8_t, uint8_t);
void sendPageState(uint8_t page);
void sendPageState(uint8_t n, uint8_t page);

void delay_print(Print* p);
void display_boot_progress(uint8_t per, const char* text = NULL);

void power_button_handler();
static void ICACHE_RAM_ATTR power_button_state_change();

// ADC_MODE(ADC_VCC);

void setup();
void loop();
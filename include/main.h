#include <Arduino.h>


void onHttpClientConnect(uint8_t);
void onHttpClientDisconnect(uint8_t);
void onHttpClientData(uint8_t n, String data);

void sendToClients(String, uint8_t);
void sendToClients(String, uint8_t, uint8_t);
void sendPageState(uint8_t page);
void sendPageState(uint8_t n, uint8_t page);

// ADC_MODE(ADC_VCC);

void setup();
void loop();
#pragma once

#include <Arduino.h>

#include <Wire.h>

#define INA231_REG_CONFIG 0x00
#define INA231_REG_SHUNT_VOL 0x01
#define INA231_REG_BUS_VOL 0x02
#define INA231_REG_POWER 0x03
#define INA231_REG_CURRENT 0x04
#define INA231_REG_CALIBRATION 0x05
#define INA231_REG_MASK_ENABLE 0x06
#define INA231_REG_ALERT_LIMIT 0x07

#define INA231_ALERT_PIN 16
#define INA231_I2C_ADDR 0x40

// bits 9-11
const uint16_t INA231_CONFIG_AVG_MASK = 0x0e00;
// bits 0-9
const uint16_t INA231_ALERT_MASK = 0x03ff;
// bits 0-3
const uint16_t INA231_CONFIG_MODE_MASK = 0x0007;

const uint8_t INA231_ALERT_CONVERSION_READY_BIT = 10;

enum INA231_AVERAGES {
    AVG_1 = 0b000,
    AVG_4 = 0b001,
    AVG_16 = 0b010,
    AVG_64 = 0b011,
    AVG_128 = 0b100,
    AVG_256 = 0b101,
    AVG_512 = 0b110,
    AVG_1024 = 0b111
};

enum INA231_CONVERSATION_TIME {
    TIME_140 = 0b000,
    TIME_204 = 0b001,
    TIME_332 = 0b010,
    TIME_558 = 0b011,
    TIME_1100 = 0b100,
    TIME_2116 = 0b101,
    TIME_4156 = 0b110,
    TIME_8244 = 0b111
};

void ina231_write_config(uint16_t value);
uint16_t ina231_read_config();

void ina231_writeWord(uint8_t addr, uint16_t value);
uint16_t ina231_readWord(uint8_t addr);

uint16_t ina231_get_config();
void ina231_set_avg(const INA231_AVERAGES avg);
void ina231_configure();
void ina231_alert_on_conversation(bool enabled);
float ina231_read_voltage();
float ina231_read_current();
float ina231_read_power();
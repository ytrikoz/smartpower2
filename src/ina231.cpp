#include <ina231.h>

#include <Wire.h>

void ina231_writeWord(uint8_t addr, uint16_t value) {
    uint8_t data[2];
    data[0] = (uint8_t)((value >> 8) & 0x00ff);
    data[1] = (uint8_t)(value & 0x00ff);

    Wire.beginTransmission(INA231_I2C_ADDR);
    Wire.write(addr);
    Wire.write(data[0]);
    Wire.write(data[1]);
    Wire.endTransmission();

    delayMicroseconds(I2C_DELAY);
}

uint16_t ina231_readWord(uint8_t addr) {
    Wire.beginTransmission(INA231_I2C_ADDR);
    Wire.write(addr);
    Wire.endTransmission();

    delayMicroseconds(I2C_DELAY);

    Wire.requestFrom(INA231_I2C_ADDR, 2);
    uint16_t ret = (uint16_t)Wire.read() << 8;
    ret |= Wire.read();

    return ret;
}

void ina231_configure(void) {
    unsigned short config;
    config = ina231_get_config();
    ina231_writeWord(INA231_REG_CONFIG, config);
    config = 0x08bd;
    ina231_writeWord(INA231_REG_CALIBRATION, config);
    config = 0x0408;
    ina231_writeWord(INA231_REG_MASK_ENABLE, config);
}

float ina231_read_voltage(void) {
    float LSB_V = 0.00125;
    unsigned short bus_vol = ina231_readWord(INA231_REG_BUS_VOL);
    return (float)(bus_vol * LSB_V);
}

float ina231_read_current(void) {
    float LSB_I = 0.0001526;
    uint16_t curr = ina231_readWord(INA231_REG_CURRENT);
    if (curr < 59)
        curr = 0;
    else if (curr > 32768)
        return 0;
    return (float)(curr * LSB_I);
}

float ina231_read_power(void) {
    float LSB_P = 0.00381687;
    uint16_t power = ina231_readWord(INA231_REG_POWER);
    if (power < 10) power = 0;
    return (float)(power * LSB_P);
}

uint16_t ina231_get_config() {
    // unsigned short res = 0x05FF & (uint8_t)numOfAvg << 8;
    // default
    return 0x45ff;
}

void ina231_set_avg(uint8_t avg) {
    int16_t config_reg = ina231_readWord(INA231_REG_CONFIG);
    config_reg |= avg << 9;
    ina231_writeWord(INA231_REG_CONFIG, config_reg);
}

void ina231_alert_on_conversation(bool enabled) {
    uint16_t alert_reg = ina231_readWord(INA231_REG_MASK_ENABLE);
    alert_reg &= INA231_ALERT_MASK;
    if (enabled) bitSet(alert_reg, INA231_ALERT_CONVERSION_READY_BIT);
    ina231_writeWord(INA231_REG_MASK_ENABLE, alert_reg);
}

void ina231_set_mode(const uint8_t mode) {
    int16_t config_reg = ina231_readWord(INA231_REG_CONFIG);
    config_reg &= ~INA231_CONFIG_MODE_MASK;
    uint16_t _mode = 0b00000111 & mode;
    config_reg |= _mode;
    ina231_writeWord(INA231_REG_CONFIG, config_reg);
}
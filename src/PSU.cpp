#include "Psu.h"

#include <FS.h>

#include "TimeProfiler.h"
#include "TimeUtils.h"
#include "ina231.h"
#include "mcp4652.h"

#define PSU_VOLTAGE_LOW 1

Psu::Psu() {
    info = PsuInfo();

    wattHoursCalculationEnabled = false;

    active = false;
    initialized = false;

    startedAt = 0;
    updatedAt = 0;
    calcedAt = 0;

    pinMode(POWER_SWITCH_PIN, OUTPUT);
}

PsuInfo Psu::getInfo() { return info; }

void Psu::togglePower() { setState(state == POWER_OFF ? POWER_ON : POWER_OFF); }

void Psu::setConfig(ConfigHelper *config) { this->config = config; }

void Psu::setOnPowerOn(PsuEventHandler h) { onPowerOn = h; }

void Psu::setOnPowerOff(PsuEventHandler h) { onPowerOff = h; }

void Psu::setOnPowerError(PsuEventHandler h) { onPowerError = h; }

void Psu::setState(PowerState value, bool force) {
    if (!force && state == value) return;

    state = value;

    output->print(FPSTR(str_psu));
    output->printf_P(strf_arrow_dest, getStateStr().c_str());
    output->println();

    digitalWrite(POWER_SWITCH_PIN, state);

    storePowerState(state);

    if (state == POWER_ON) {
        onStart();
    } else if (state == POWER_OFF) {
        onStop();
    }
}

PowerState Psu::getState() { return state; }

float Psu::getOutputVoltage() { return outputVoltage; }

void Psu::setOutputVoltage(float value) {
    output->print(FPSTR(str_psu));
    output->print(FPSTR(str_set));
    output->printf_P(strf_output_voltage, value);
    output->println();

    outputVoltage = value;
    mcp4652_write(WRITE_WIPER0, quadraticRegression(value));
}

void Psu::begin() {
    if (!initialized) init();

    float v = config->getOutputVoltage();
    this->setOutputVoltage(v);

    PowerState ps;
    switch (config->getBootPowerState()) {
        case BOOT_POWER_OFF:
            ps = POWER_OFF;
            break;
        case BOOT_POWER_ON:
            ps = POWER_ON;
            break;
        case BOOT_POWER_LAST_STATE:
            ps = restorePowerState();
            break;
        default:
            ps = POWER_OFF;
    }

    setState(ps, true);
}

void Psu::storePowerState(PowerState state) {
    File f = SPIFFS.open(FILE_LAST_POWER_STATE, "w");
    f.println(state);
    f.flush();
    f.close();
}

PowerState Psu::restorePowerState() {
    if (!SPIFFS.exists(FILE_LAST_POWER_STATE)) {
        storePowerState(POWER_OFF);
        return POWER_OFF;
    }
    File f = SPIFFS.open(FILE_LAST_POWER_STATE, "r");
    PowerState value = PowerState(f.readString().toInt());
    f.close();
    return value;
}

void Psu::onStart() {
    startedAt = millis();
    updatedAt = startedAt;
    calcedAt = startedAt;

    clearError();

    if (onPowerOn) onPowerOn();

    active = true;
}

void Psu::onStop() {
    if (onPowerOff) onPowerOff();

    active = false;
}

void Psu::loop() {
    if (!active) return;
    unsigned long now = millis();
    if (millis_passed(updatedAt, now) >= MEASUREMENT_INTERVAL_ms) {
        info.voltage = ina231_read_voltage();
        info.current = ina231_read_current();
        updatedAt = now;
    }

    unsigned long sinceLastCalc = millis_passed(calcedAt, now);
    if (sinceLastCalc >= ONE_SECOND_ms) {
        info.power = ina231_read_power();
        if (wattHoursCalculationEnabled)
            info.wattSeconds += info.power * sinceLastCalc / ONE_SECOND_ms;
        calcedAt = millis();
    }

    if (millis_passed(startedAt, now) > 5000 &&
        info.voltage <= PSU_VOLTAGE_LOW) {
        output->print(FPSTR(str_psu));
        output->print(FPSTR(str_error));
        output->print(FPSTR(str_low_voltage));
        output->print(info.voltage);
        output->println();
        error(PSU_ERROR_LOW_VOLTAGE);
    }
}

void Psu::clearError() { status = PSU_OK; }

void Psu::error(PsuStatus err) {
    status = err;

    setState(POWER_OFF);

    if (onPowerError) onPowerError();
}

void Psu::setWattHours(double value) { info.wattSeconds = value / ONE_HOUR_s; }

void Psu::enableWattHoursCalculation(bool enabled) {
    wattHoursCalculationEnabled = enabled;
}

bool Psu::isWattHoursCalculationEnabled() {
    return wattHoursCalculationEnabled;
}

float Psu::getVoltage() { return info.voltage; }

float Psu::getCurrent() { return info.current; }

float Psu::getPower() { return info.power; }

double Psu::getWattHours() { return info.wattSeconds * ONE_HOUR_s; }

String Psu::toString() {
    String str = String(getVoltage(), 3);
    str += "V, ";
    str += String(getCurrent(), 3);
    str += "A, ";
    str += String(getPower(), 3);
    str += "W, ";
    str += String(getWattHours(), 3);
    str += "Wh";
    return str;
}

unsigned long Psu::getDuration() {
    return millis_passed(startedAt, updatedAt) / ONE_SECOND_ms;
}

void Psu::init() {
    // meter
    ina231_configure();
    // pot
    mcp4652_init();

    initialized = true;
}

String Psu::getStateStr() { return getState() == POWER_ON ? "ON " : "OFF "; }

void Psu::printDiag(Print *p) {
    p->print(FPSTR(str_psu));
    switch (status) {
        case PSU_OK:
        p->print(getStateStr());    
            if (getState() == POWER_ON) {
                p->printf_P(strf_output_voltage, getOutputVoltage());
                p->printf_P(strf_lu_sec, getDuration());
            } else if (getState() == POWER_OFF) {
                p->printf_P(strf_lu_sec, millis_since(updatedAt) / ONE_SECOND_ms);
            }
            p->println();
            break;
        case PSU_ERROR_LOW_VOLTAGE:
            p->print(FPSTR(str_error));
            p->print(FPSTR(str_low_voltage));
            p->println(info.voltage);
            break;
        default:
            break;
            return;
    }
}
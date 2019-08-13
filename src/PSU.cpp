#include "Psu.h"

#include <FS.h>

#include "TimeProfiler.h"
#include "TimeUtils.h"
#include "ina231.h"
#include "mcp4652.h"

Psu::Psu() {
    info = PsuInfo();

    wattHoursCalculationEnabled = false;

    active = false;
    initialized = false;

    startedAt = 0;
    lastUpdated = 0;
    lastPowerRead = 0;

    pinMode(POWER_SWITCH_PIN, OUTPUT);
}

PsuInfo Psu::getInfo() { return info; }

void Psu::togglePower() { setState(state == POWER_OFF ? POWER_ON : POWER_OFF); }

void Psu::setConfig(ConfigHelper *config) { this->config = config; }

void Psu::setOnOn(PsuEventHandler h) { onPsuOn = h; }

void Psu::setOnOff(PsuEventHandler h) { onPsuOff = h; }

void Psu::setOnError(PsuEventHandler h) { onPsuError = h; }

void Psu::setOnAlert(PsuEventHandler h) { onPsuAlert = h; }

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
    output->printf_P(strf_output_voltage, value);
    output->println();

    outputVoltage = value;
    
    mcp4652_write(WRITE_WIPER0_ADDR, quadratic_regression(value));
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

    lastUpdated = startedAt;
    lastPowerRead = startedAt;
    lastAlertCheck = startedAt;

    clearError();

    if (onPsuOn) onPsuOn();

    active = true;
}

void Psu::onStop() {
    if (onPsuOff) onPsuOff();

    active = false;
}

void Psu::loop() {
    if (!active) return;

    unsigned long now = millis();
    if (millis_passed(lastUpdated, now) >= MEASUREMENT_INTERVAL_ms) {
        info.voltage = ina231_read_voltage();
        info.current = ina231_read_current();
        lastUpdated = now;
    }

    unsigned long sinceLastPowerRead = millis_passed(lastPowerRead, now);    
    if (sinceLastPowerRead >= ONE_SECOND_ms) {
        info.power = ina231_read_power();
        if (wattHoursCalculationEnabled)
            info.wattSeconds += info.power * ((float) sinceLastPowerRead / ONE_SECOND_ms);
        lastPowerRead = now;
    }
    
    if (status == PSU_OK) {
        if (millis_passed(lastAlertCheck, now) > PSU_CHECK_INTERVAL_s * ONE_SECOND_ms) {
            if (info.voltage <= PSU_VOLTAGE_LOW_v) {
                alert(PSU_ALERT_VOLTAGE_LOW);
            } else if (info.current <= PSU_LOAD_LOW_a) {
                alert(PSU_ALERT_LOAD_LOW);
            }
        }
    }
}

void Psu::clearError() { status = PSU_OK; }

void Psu::alert(PsuAlert a) {
    status = PSU_ALERT;
    
    output->print(FPSTR(str_psu));
    output->print(FPSTR(str_alert));
    switch (a) {
        case PSU_ALERT_VOLTAGE_LOW:        
        {
            output->print(FPSTR(str_low_voltage));
            output->print(info.voltage);
        }
        case PSU_ALERT_LOAD_LOW:
        {
            output->print(FPSTR(str_load_low));
            output->print(info.current);
        }
    }
    output->println();
    if (onPsuAlert) onPsuAlert();
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
    return floor((float) millis_passed(startedAt, lastUpdated) / ONE_SECOND_ms);
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
    p->print(getStateStr());    
    p->printf_P(strf_output_voltage, getOutputVoltage());         
    if (getState() == POWER_ON) {
        p->printf_P(strf_lu_sec, getDuration());        
    } else if (getState() == POWER_OFF) {
        p->printf_P(strf_lu_sec, millis_since(lastUpdated) / ONE_SECOND_ms);
    }   
    switch (status)
    {
        case PSU_OK:
            p->print(FPSTR(str_ok));
            break;
        case PSU_ERROR:
            p->print(FPSTR(str_error));
            break;
        case PSU_ALERT:
            p->print(FPSTR(str_alert));
            break;
        default:
            break;
        return;
    }
    p->println();
}
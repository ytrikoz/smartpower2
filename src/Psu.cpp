#include "Psu.h"

#include <FS.h>

#include "Consts.h"
#include "StoreUtils.h"
#include "TimeUtils.h"
#include "ina231.h"
#include "mcp4652.h"

Psu::Psu() {
    info = PsuInfo();

    wh_store = false;

    active = false;
    initialized = false;

    startedAt = 0;
    lastUpdated = 0;
    lastPowerRead = 0;

    pinMode(POWER_SWITCH_PIN, OUTPUT);

    clearError();
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
    output->print(getIdentStrP(str_psu));
    output->print(getStateInfo());
    state = value;
    output->print(getStrP(str_arrow_dest));
    output->println(getStateInfo());

    digitalWrite(POWER_SWITCH_PIN, state);

    if (state == POWER_ON) {
        if (wh_store) restoreWh(info.mWh);
        onStart();
    }
    else if (state == POWER_OFF) {
        if (wh_store) storeWh(info.mWh);
        onStop();
    }
    storeState(state);
}

PowerState Psu::getState() { return state; }

float Psu::getOutputVoltage() { return outputVoltage; }

void Psu::setOutputVoltage(float value) {
    output->print(getIdentStrP(str_psu));
    output->print(getStrP(str_arrow_dest));
    output->printf_P(strf_output_voltage, value);
    output->println();

    outputVoltage = value;

    mcp4652_write(WRITE_WIPER0_ADDR, quadratic_regression(value));
}

void Psu::begin() {
    if (!initialized) init();

    float v = config->getOutputVoltage();
    this->setOutputVoltage(v);

    wh_store = false;
    if (config->getWatthHoursLogEnabled()) {
        if (!restoreWh(info.mWh)) {
            info.mWh = 0;
            wh_store = storeWh(info.mWh);
        }
    }
    
    PowerState ps;
    switch (config->getBootPowerState()) {
        case BOOT_POWER_OFF:
            ps = POWER_OFF;
            break;
        case BOOT_POWER_ON:
            ps = POWER_ON;
            break;
        case BOOT_POWER_LAST_STATE:
            PowerState stored_state;
            if (restoreState(stored_state)) ps = stored_state; else ps = POWER_OFF;
            break;
        default:
            ps = POWER_OFF;
    }

    setState(ps, true);
}

void Psu::onStart() {
    startedAt = millis();

    lastUpdated = startedAt;
    lastPowerRead = startedAt;
    lastCheck = startedAt;

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
        lastUpdated = now;
        info.V = ina231_read_voltage();
        info.I = ina231_read_current();
        info.time = lastUpdated;
    }
 
    unsigned long sincePowerRead = millis_passed(lastPowerRead, now);
    if (sincePowerRead >= ONE_SECOND_ms) {
        lastPowerRead = now;
        info.P = ina231_read_power();
        info.mWh += (float) (sincePowerRead / ONE_SECOND_ms) * info.P / ONE_HOUR_ms;
        info.time = now;
    }

    if (status == PSU_OK) {
        if (millis_passed(lastCheck, now) >
            PSU_CHECK_INTERVAL_s * ONE_SECOND_ms) {
            if (info.V <= PSU_VOLTAGE_LOW_v) {
                setAlert(PSU_ALERT_VOLTAGE_LOW);
            } else if (info.I <= PSU_LOAD_LOW_a) {
                setAlert(PSU_ALERT_LOAD_LOW);
            }
        }
    }
}

void Psu::clearError() {
    status = PSU_OK;
    alert = PSU_ALERT_NONE;
}

void Psu::setAlert(PsuAlert a) {
    status = PSU_ALERT;
    alert = a;
    if (onPsuAlert) onPsuAlert();
}

void Psu::setWh(double value) {    
    info.mWh = value * ONE_WATT_mW;
    if (wh_store) storeWh(info.mWh);
}

bool Psu::storeWh(double value ) {
    return StoreUtils::storeDouble(FILE_VAR_WH, value);    
}

bool Psu::restoreWh(double &value) {
    return StoreUtils::restoreDouble(FILE_VAR_WH, value);
}

bool Psu::enableWhStore(bool new_value) {    
    if (wh_store == new_value) return true;
    wh_store = new_value == true ? storeWh(info.mWh) : false;    
    return wh_store == new_value;
}

bool Psu::isWattHoursLogEnabled() { return wh_store; }

float Psu::getP() { return info.P; }

float Psu::getV() { return info.V; }

float Psu::getI() { return info.I; }

double Psu::getWh() { return info.mWh / ONE_WATT_mW; }

String Psu::toString() {
    String str = String(getV(), 3);
    str += "V, ";
    str += String(getI(), 3);
    str += "A, ";
    str += String(getP(), 3);
    str += "W, ";
    str += String(getWh(), 3);
    str += "Wh";
    return str;
}

void Psu::init() {
    // meter
    ina231_configure();
    // pot
    mcp4652_init();

    initialized = true;
}

unsigned long Psu::getDuration() {
    return floor((float)millis_passed(startedAt, lastUpdated) / ONE_SECOND_ms);
}

String Psu::getAlertInfo() {
    String str = "";
    if (status == PSU_ALERT) {
        str = getStrP(str_alert);
        switch (alert) {
            case PSU_ALERT_VOLTAGE_LOW:
                str += getStrP(str_low_voltage, false);
                break;
            case PSU_ALERT_LOAD_LOW:
                str += getStrP(str_load_low, false);
                break;
            default:
                str += getStrP(str_unknown);
                break;
        }
    }
    return str;
}

String Psu::getDurationInfo() {
    char buf[32];
    sprintf_P(buf, strf_lu_sec, getDuration());
    return buf;
}

String Psu::getOutputVoltageInfo() {
    char buf[32];
    sprintf_P(buf, strf_output_voltage, getOutputVoltage());
    return buf;
}

bool Psu::storeState(PowerState value) {
    return StoreUtils::storeInt(FILE_VAR_POWER_STATE, (byte) value);
}

bool Psu::restoreState(PowerState &value) {
    int buf;    
    bool res = StoreUtils::restoreInt(FILE_VAR_POWER_STATE, buf);
    if (res) value = PowerState(buf);
    return res;
}

String Psu::getStateInfo() {
    String str = "";
    if (state == POWER_ON) {
        str = getStrP(str_on);
    } else if (state == POWER_OFF) {
        str = getStrP(str_off);
    }
    return str;
}

void Psu::printDiag(Print *p) {
    p->print(getStrP(str_power));
    p->println(getStateInfo());
    p->println(getOutputVoltageInfo());
    if (state == POWER_ON) p->println(getDurationInfo());
    switch (status) {
        case PSU_ALERT:
            p->println(getAlertInfo());
            break;
        case PSU_ERROR:
            p->println(getStrP(str_error));
            break;
        default:
            break;
    }
}
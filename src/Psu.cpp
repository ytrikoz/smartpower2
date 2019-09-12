#include "Psu.h"

#include <FS.h>

#include "Consts.h"
#include "StoreUtils.h"
#include "TimeUtils.h"
#include "ina231.h"
#include "mcp4652.h"

Psu::Psu() {
    info = PsuInfo();
    psuState = PsuState();

    wh_store = false;
    startTime = 0;
    infoUpdated = 0;
    powerInfoUpdated = 0;

    pinMode(POWER_SWITCH_PIN, OUTPUT);
    
    active = false;
    initialized = false;
}

PsuInfo Psu::getInfo() { return info; }

void Psu::togglePower() {   
    setState(state == POWER_OFF ? POWER_ON : POWER_OFF);

    if (onTogglePower) onTogglePower();
}

void Psu::setConfig(ConfigHelper *config) { this->config = config; }

void Psu::setOnTogglePower(PsuEventHandler h) { onTogglePower = h; }

void Psu::setOnPowerOn(PsuEventHandler h) { onPowerOn = h; }

void Psu::setOnPowerOff(PsuEventHandler h) { onPowerOff = h; }

void Psu::setOnError(PsuEventHandler h) { onPsuError = h; }

void Psu::setOnAlert(PsuEventHandler h) { onPsuAlert = h; }

void Psu::setState(PowerState value, bool force) {
    if (!force && state == value) return;

    out->print(getIdentStrP(str_psu));
    out->print(getStateStr(state));
    state = value;
    out->print(getStrP(str_arrow_dest));
    out->println(getStateStr(value));

    digitalWrite(POWER_SWITCH_PIN, state);

    if (state == POWER_ON) {
        if (wh_store) restoreWh(info.mWh);
        onStart();
    } else if (state == POWER_OFF) {
        if (wh_store) storeWh(info.mWh);
        onStop();
    }

    storeState(state);
}

bool Psu::isWhStoreEnabled() { return wh_store; }

PowerState Psu::getState() { return state; }

float Psu::getOutputVoltage() { return outputVoltage; }

void Psu::setOutputVoltage(float value) {
    outputVoltage = constrain(value, 4, 6);

    out->print(getIdentStrP(str_psu));
    out->print(getStrP(str_set));
    out->print(getStrP(str_output));
    out->print(getStrP(str_voltage));
    out->print(outputVoltage, 2);
    if (value != outputVoltage) {
        out->print('(');
        out->print(value, 2);
        out->print(')');
    }
    out->println();

    mcp4652_write(WRITE_WIPER0_ADDR, quadratic_regression(outputVoltage));
}

void Psu::begin() {
    if (!initialized) init();
    double v = config->getOutputVoltage();
    this->setOutputVoltage(v);
    wh_store = false;
    if (config->getWhStoreEnabled()) {
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
            if (restoreState(stored_state))
                ps = stored_state;
            else
                ps = POWER_OFF;
            break;
        default:
            ps = POWER_OFF;
    }

    setState(ps, true);
}

void Psu::onStart() {
    startTime = infoUpdated = powerInfoUpdated = lastCheck = millis();

    psuState.clear();

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

    if (millis_passed(infoUpdated, now) >= MEASUREMENT_INTERVAL_ms) {
        infoUpdated = now;
        info.V = ina231_read_voltage();
        info.I = ina231_read_current();
        info.time = now;
    }

    unsigned long sincePowerReaded = millis_passed(powerInfoUpdated, now);
    if (sincePowerReaded >= ONE_SECOND_ms) {
        info.P = ina231_read_power();
        powerInfoUpdated = now;
        info.mWh +=
            (float)sincePowerReaded / ONE_SECOND_ms * info.P / ONE_HOUR_ms;
        info.time = now;
    }

    if (psuState.isOK()) {
        if (millis_passed(lastCheck, now) >
            PSU_CHECK_INTERVAL_s * ONE_SECOND_ms) {
            if (info.V < PSU_VOLTAGE_LOW_v) {
               psuState.setError(PSU_ERROR_DC_IN_LOW);
            } else if (info.I <= PSU_LOAD_LOW_a) {
                psuState.setAlert(PSU_ALERT_LOAD_LOW);
            } else {
                psuState.clear();
            }
        }
    }
}

void Psu::setWh(double value) {
    info.mWh = value * ONE_WATT_mW;
    if (wh_store) storeWh(info.mWh);
}

bool Psu::storeWh(double value) {
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

float Psu::getP() { return info.P; }

float Psu::getV() { return info.V; }

float Psu::getI() { return info.I; }

double Psu::getWh() { return info.mWh / ONE_WATT_mW; }

void Psu::init() {
    // meter
    ina231_configure();
    // pot
    mcp4652_init();

    initialized = true;
}

unsigned long Psu::getUptime() {
    return millis_passed(startTime, infoUpdated) / ONE_SECOND_ms;
}

bool Psu::storeState(PowerState value) {
    return StoreUtils::storeInt(FILE_VAR_POWER_STATE, (byte)value);
}

bool Psu::restoreState(PowerState &value) {
    int buf;
    bool res = StoreUtils::restoreInt(FILE_VAR_POWER_STATE, buf);
    if (res) value = PowerState(buf);
    return res;
}

void Psu::printDiag(Print *p) {
    p->println(psuState);    
    p->print(getStrP(str_power));
    p->println(getStateStr(state));
    p->print(getStrP(str_output));
    p->print(getStrP(str_voltage));
    p->println(getOutputVoltage(), 2);    
    if (state == POWER_ON) {    
        p->print(getUptime());
        p->println(getStrP(str_sec, false));        
    }
 
}
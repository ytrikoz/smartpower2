#include "Modules/PsuModule.h"

#include <FS.h>

#include "PrintUtils.h"
#include "PsuUtils.h"
#include "StoreUtils.h"
#include "TimeUtils.h"


using namespace PrintUtils;
using namespace StrUtils;
using namespace StoreUtils;

int Psu::quadratic_regression(double value) {
    double a = 0.0000006562;
    double b = 0.0022084236;
    float c = alterRange ? 11.83 : 4.08;
    double d = b * b - a * (c - value);
    double root = (-b + sqrt(d)) / a;
    if (root < 0)
        root = 0;
    else if (root > 255)
        root = 255;
    return root;
}

bool Psu::onInit() {
    setVoltage(config_->getValueAsFloat(OUTPUT_VOLTAGE));
    enableWhStore(config_->getValueAsBool(WH_STORE_ENABLED));
    startTime = infoUpdated = powerInfoUpdated = loggerUpdated = lastStore = 0;
    pinMode(POWER_SWITCH_PIN, OUTPUT);
    clearErrorsAndAlerts();
    return true;
}

void Psu::powerOn() {
    startTime = infoUpdated = powerInfoUpdated = lastCheck = millis();
    info.mWh = 0;
    if (isWhStoreEnabled())
        restoreWh(info.mWh);
    setState(POWER_ON);
}

void Psu::powerOff() {
    if (wh_store)
        storeWh(info.mWh);
    setState(POWER_OFF);
}

void Psu::setState(PsuState value) {
    if (state == value)
        return;
    state = value;
    digitalWrite(POWER_SWITCH_PIN, value);
    clearErrorsAndAlerts();
    storeState(state);
    if (stateChangeHandler)
        stateChangeHandler(state, status);
}

bool Psu::checkVoltageRange(double value) { return alterRange = value > 7; }

void Psu::setVoltage(double value) {
    if (checkVoltageRange(value))
        outputVoltage = constrain(value, 11.8, 12.9);
    else
        outputVoltage = constrain(value, 4.1, 5.3);

    mcp4652_write(WRITE_WIPER0_ADDR, quadratic_regression(value));
}

bool Psu::onStart() {
    PsuState ps;
    switch (config_->getValueAsByte(POWER)) {
    case BOOT_POWER_OFF:
        ps = POWER_OFF;
        break;
    case BOOT_POWER_ON:
        ps = POWER_ON;
        break;
    case BOOT_POWER_LAST_STATE:
        PsuState stored_state;
        if (restoreState(stored_state))
            ps = stored_state;
        else
            ps = POWER_OFF;
        break;
    default:
        ps = POWER_OFF;
    }
    if (ps == POWER_ON)
        powerOn();
    else
        powerOff();
    return true;
}

void Psu::onLoop() {
    if (!checkState(POWER_ON))
        return;

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

        if (psuInfoHandler)
            psuInfoHandler(info);
    }

    if (millis_passed(loggerUpdated, now) >= PSU_LOG_INTERVAL_ms) {
        PsuInfo pi = getInfo();
        logger_->log(pi);
        loggerUpdated = now;
    }

    if (millis_passed(lastCheck, now) > PSU_CHECK_INTERVAL_s * ONE_SECOND_ms) {
        if (info.V < PSU_VOLTAGE_LOW_v) {
            setError(PSU_ERROR_LOW_VOLTAGE);
        } else if (info.I <= PSU_LOAD_LOW_a) {
            setAlert(PSU_ALERT_LOAD_LOW);
        } else {
            clearErrorsAndAlerts();
        }
        lastCheck = now;
    }

    if (millis_passed(lastStore, now) > ONE_MINUTE_ms) {
        if (wh_store)
            storeWh(info.mWh);
        lastStore = now;
    }
}

void Psu::setWh(double value) {
    info.mWh = value * ONE_WATT_mW;
    if (wh_store)
        storeWh(info.mWh);
}

bool Psu::storeWh(double value) {
    bool res = storeDouble(FS_WH_VAR, value);
    return res;
}

bool Psu::enableWhStore(bool new_value) {
    if (wh_store == new_value)
        return true;
    wh_store = new_value ? restoreWh(info.mWh) : false;
    println_moduleP_nameP_value(out, str_psu, str_store,
                                getEnabledStr(wh_store).c_str());
    return wh_store == new_value;
}

bool Psu::restoreWh(double &value) {
    bool res = false;
    if (restoreDouble(FS_WH_VAR, value)) {
        print_ident(out, FPSTR(str_psu));
        println_nameP_value(out, str_restore, value);
        res = true;
    }
    return res;
}

const float Psu::getP() { return info.P; }

const float Psu::getV() { return info.V; }

const float Psu::getI() { return info.I; }

const double Psu::getWh() { return info.mWh / ONE_WATT_mW; }

const unsigned long Psu::getUptime() {
    return millis_passed(startTime, infoUpdated) / ONE_SECOND_ms;
}

bool Psu::isWhStoreEnabled(void) {
    wh_store = config_->getValueAsBool(WH_STORE_ENABLED);
    return wh_store;
}

bool Psu::storeState(PsuState value) {
    return StoreUtils::storeInt(FS_POWER_STATE_VAR, (byte) value);
}

bool Psu::restoreState(PsuState &value) {
    byte buf;
    bool res = StoreUtils::restoreByte(FS_POWER_STATE_VAR, buf);
    if (res) value = (PsuState) buf;
    return res;
}

size_t Psu::onDiag(Print *p) {
    size_t n = println_nameP_value(p, str_power, getStateStr(state));
    n += println_nameP_value(p, str_status, getStatusStr(status));
    n += println_nameP_value(p, str_output, outputVoltage);
    n += println_nameP_value(p, str_store, getBoolStr(wh_store));
    n += println_nameP_value(p, str_mod, getBoolStr(alterRange));
    if (checkState(POWER_ON))
        n += println_nameP_value(p, str_uptime, getUptime());
    return n;
}

void Psu::togglePower() {
    if (checkState(POWER_ON))
        powerOff();
    else
        powerOn();
}

PsuStatus Psu::getStatus(void) { return status; }

PsuState Psu::getState(void) { return state; }

PsuError Psu::getError(void) { return error; }

PsuAlert Psu::getAlert(void) { return alert; }

bool Psu::checkState(PsuState value) { return state == value; }

bool Psu::checkStatus(PsuStatus value) { return status == value; }

void Psu::setOnStateChange(PsuStateChangeHandler h) { stateChangeHandler = h; };

void Psu::setOnPsuInfo(PsuInfoHandler h) { psuInfoHandler = h; };

void Psu::setError(PsuError value) {
    error = value;
    setStatus(PSU_ERROR);
}

void Psu::setAlert(PsuAlert value) {
    alert = value;
    setStatus(PSU_ALERT);
}

void Psu::setStatus(PsuStatus value) {
    if (status == value)
        return;
    status = value;
    if (stateChangeHandler)
        stateChangeHandler(state, status);
}

const PsuInfo Psu::getInfo() { return info; }

const float Psu::getVoltage() { return outputVoltage; }

void Psu::clearErrorsAndAlerts() {
    alert = PSU_ALERT_NONE;
    error = PSU_ERROR_NONE;
    setStatus(PSU_OK);
}
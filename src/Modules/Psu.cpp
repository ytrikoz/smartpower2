#include "Modules/Psu.h"

#include <FS.h>

#include "AppModule.h"
#include "Consts.h"
#include "PrintUtils.h"
#include "PsuUtils.h"
#include "StoreUtils.h"
#include "TimeUtils.h"
#include "ina231.h"
#include "mcp4652.h"

using namespace PrintUtils;
using namespace StrUtils;
using namespace StoreUtils;

Psu::Psu() : AppModule(MOD_PSU) {
    pinMode(POWER_SWITCH_PIN, OUTPUT);

    ina231_configure();
    mcp4652_init();

    startTime = infoUpdated = powerInfoUpdated = loggerUpdated = 0;

    setOk();
}

void Psu::setConfig(Config *config) {
    setVoltage(config->getValueAsFloat(OUTPUT_VOLTAGE));
}

void Psu::powerOn() {
    startTime = infoUpdated = powerInfoUpdated = lastCheck = millis();

    logger->clear();

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

    setOk();

    storeState(state);

    if (stateChangeHandler)
        stateChangeHandler(state, status);
}

void Psu::setVoltage(float value) {
    outputVoltage = value;
    mcp4652_write(WRITE_WIPER0_ADDR, quadratic_regression(value));
}

bool Psu::begin() {

    wh_store = false;
    if (config->getValueAsBool(WH_STORE_ENABLED))
        if (!restoreWh(info.mWh))
            wh_store = storeWh(info.mWh);

    PsuState ps;
    switch (config->getValueAsByte(POWER)) {
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

void Psu::loop() {
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
        logger->log(pi);
        loggerUpdated = now;
    }

    if (millis_passed(lastCheck, now) > PSU_CHECK_INTERVAL_s * ONE_SECOND_ms) {
        if (info.V < PSU_VOLTAGE_LOW_v) {
            setError(PSU_ERROR_LOW_VOLTAGE);
        } else if (info.I <= PSU_LOAD_LOW_a) {
            setAlert(PSU_ALERT_LOAD_LOW);
        } else {
            setOk();
        }
        lastCheck = now;
    }
}

void Psu::setWh(double value) {
    info.mWh = value * ONE_WATT_mW;
    if (wh_store)
        storeWh(info.mWh);
}

bool Psu::enableWhStore(bool new_value) {
    if (wh_store == new_value)
        return true;
    wh_store = new_value ? storeWh(info.mWh) : false;
    printlm_moduleP_nameP_value(dbg, str_psu, str_total, wh_store);
    return wh_store == new_value;
}

bool Psu::storeWh(double value) {
    return StoreUtils::storeDouble(FILE_VAR_WH, value);
}

bool Psu::restoreWh(double &value) {
    return ::restoreDouble(FILE_VAR_WH, value);
}

float Psu::getP() { return info.P; }

float Psu::getV() { return info.V; }

float Psu::getI() { return info.I; }

double Psu::getWh() { return info.mWh / ONE_WATT_mW; }

unsigned long Psu::getUptime() {
    return millis_passed(startTime, infoUpdated) / ONE_SECOND_ms;
}

bool Psu::storeState(PsuState value) {
    return StoreUtils::storeInt(FILE_VAR_POWER_STATE, (byte)value);
}

bool Psu::restoreState(PsuState &value) {
    int tmp = 0;
    bool res = StoreUtils::restoreInt(FILE_VAR_POWER_STATE, tmp);
    if (res)
        value = PsuState(tmp);
    return res;
}

size_t Psu::printDiag(Print *p) {
    size_t n = println_nameP_value(p, str_power, getStateStr(state));
    n += println_nameP_value(p, str_status, getStatusStr(status));
    n += println_nameP_value(p, str_output, outputVoltage);
    if (checkState(POWER_ON))
        n += PrintUtils::println_nameP_value(p, str_uptime, getUptime());
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

void Psu::setLogger(PsuLogger *logger) { this->logger = logger; }

PsuLogger *Psu::getLogger() { return this->logger; }

PsuInfo Psu::getInfo() { return info; }

bool Psu::isWhStoreEnabled(void) {
    wh_store = false;
    if (config->getValueAsBool(WH_STORE_ENABLED))
        if (!restoreWh(info.mWh))
            wh_store = storeWh(info.mWh);
    return wh_store;
}

float Psu::getVoltage() { return outputVoltage; }

void Psu::setOk(void) {
    alert = PSU_ALERT_NONE;
    error = PSU_ERROR_NONE;
    setStatus(PSU_OK);
}
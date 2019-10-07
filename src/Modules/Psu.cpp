#include "Modules/Psu.h"

#include <FS.h>

#include "AppModule.h"
#include "Consts.h"
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

    reset();

    wh_store = false;

    startTime = infoUpdated = powerInfoUpdated = loggerUpdated = 0;
}

void Psu::setLogger(PsuLogger *logger) { this->logger = logger; }

PsuLogger *Psu::getLogger() { return this->logger; }

PsuInfo Psu::getInfo() { return info; }

void Psu::powerOn() {
    reset();
    logger->clear();
    if (wh_store)
        restoreWh(info.mWh);
    startTime = infoUpdated = powerInfoUpdated = lastCheck = millis();
    setState(POWER_ON, true);
}

void Psu::powerOff() {
    if (wh_store)
        storeWh(info.mWh);
    setState(POWER_OFF, true);
}

void Psu::setState(PsuState value, bool force) {
    if (!force && state == value)
        return;

    digitalWrite(POWER_SWITCH_PIN, value);

    state = value;

    if (stateHandler)
        stateHandler(state);
}

bool Psu::isWhStoreEnabled() { return wh_store; }

float Psu::getVoltage() { return outputVoltage; }

void Psu::setVoltage(float value) {
    outputVoltage = constrain(value, 4, 6);
    mcp4652_write(WRITE_WIPER0_ADDR, quadratic_regression(outputVoltage));
}

String Psu::getMessage() {
    PGM_P strP = str_unknown;
    switch (status) {
    case PSU_OK:
        strP = str_ok;
    case PSU_ALERT:
        strP = getAlertStrP(alert);
        break;
    case PSU_ERROR:
        strP = getErrorStrP(error);
        break;
    }
    return StrUtils::getStrP(strP);
};

void Psu::setConfig(Config *config) {
    setVoltage(config->getValueAsFloat(OUTPUT_VOLTAGE));
}

bool Psu::begin() {
    wh_store = false;
    if (config->getValueAsBool(WH_STORE_ENABLED)) {
        if (!restoreWh(info.mWh)) {
            info.mWh = 0;
            wh_store = storeWh(info.mWh);
        }
    }

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

    setState(ps, true);

    return true;
}

void Psu::end() { powerOff(); }

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
            reset();
        }
    }
}

void Psu::setWh(double value) {
    info.mWh = value * ONE_WATT_mW;
    if (wh_store)
        storeWh(info.mWh);
}

bool Psu::storeWh(double value) {
    return StoreUtils::storeDouble(FILE_VAR_WH, value);
}

bool Psu::restoreWh(double &value) {
    return ::restoreDouble(FILE_VAR_WH, value);
}

bool Psu::enableWhStore(bool new_value) {
    if (wh_store == new_value)
        return true;
    wh_store = new_value == true ? storeWh(info.mWh) : false;
    return wh_store == new_value;
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
    size_t n = print_nameP_value(p, str_power, getStateStr().c_str());
    n += print_nameP_value(p, str_output, outputVoltage);
    if (checkState(POWER_ON))
        n += PrintUtils::print_nameP_value(p, str_uptime, getUptime());
    return n;
}

void Psu::togglePower() {
    if (checkState(POWER_ON))
        powerOff();
    else
        powerOn();
}

PsuStatus Psu::getStatus() { return status; }

bool Psu::checkState(PsuState value) { return state == value; }

bool Psu::checkStatus(PsuStatus value) { return status == value; }

PsuState Psu::getState(void) { return state; }

void Psu::setState(PsuState value) {
    state = value;
    updateStatus();
}

String Psu::getStateStr() {
    return StrUtils::getStrP(state == POWER_ON ? str_on : str_off);
}

void Psu::setOnStatusChange(PsuStatusHandler h) { statusHandler = h; };

void Psu::setOnStateChange(PsuStateHandler h) { stateHandler = h; };

void Psu::setError(PsuError value) {
    error = value;
    updateStatus();
}

void Psu::setAlert(PsuAlert value) {
    alert = value;
    updateStatus();
}

PGM_P Psu::getAlertStrP(PsuAlert value) {
    PGM_P str;
    switch (value) {
    case PSU_ALERT_LOAD_LOW:
        str = str_load_low;
        break;
    default:
        str = str_unknown;
        break;
    }
    return str;
}

PGM_P Psu::getErrorStrP(PsuError value) {
    PGM_P str;
    switch (value) {
    case PSU_ERROR_LOW_VOLTAGE:
        str = str_low_voltage;
        break;
    default:
        str = str_unknown;
        break;
    }
    return str;
}

void Psu::updateStatus() {
    PsuStatus before = status;
    if (error) {
        status = PSU_ERROR;
    } else if (alert) {
        status = PSU_ALERT;
    } else {
        status = PSU_OK;
    }

    if (status == before)
        return;

    PGM_P strP;
    switch (status) {
    case PSU_OK:
        strP = str_ok;
        break;
    case PSU_ERROR:
        strP = str_error;
        break;
    case PSU_ALERT:
        strP = str_alert;
        break;
    default:
        strP = str_unknown;
        break;
    }

    String str = String(FPSTR(strP));
    if (statusHandler)
        statusHandler(status, str);
}

void Psu::reset(void) {
    status = PSU_OK;
    alert = PSU_ALERT_NONE;
    error = PSU_ERROR_NONE;
}
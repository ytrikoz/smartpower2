#include "Modules/Psu.h"

#include <FS.h>

#include "AppModule.h"
#include "Consts.h"
#include "StoreUtils.h"
#include "TimeUtils.h"
#include "ina231.h"
#include "mcp4652.h"

using namespace PrintUtils;

Psu::Psu() : AppModule(MOD_PSU) {
    info = PsuInfo();
    psuState = PsuState();

    wh_store = false;
    startTime = infoUpdated = powerInfoUpdated = loggerUpdated = 0;

    pinMode(POWER_SWITCH_PIN, OUTPUT);

    ina231_configure();
    mcp4652_init();

    active = false;
}

void Psu::setLogger(PsuLogger *logger) { this->logger = logger; }

PsuLogger *Psu::getLogger() { return this->logger; }

PsuInfo Psu::getInfo() { return info; }

void Psu::togglePower() {
    if (getPsuState()->getPower(POWER_ON)) {
        powerOff();
    } else {
        powerOn();
    }
    if (togglePowerHandler)
        togglePowerHandler();
}

void Psu::setOnTogglePower(PsuEventHandler h) { togglePowerHandler = h; }

void Psu::setOnPowerOn(PsuEventHandler h) { powerOnHandler = h; }

void Psu::setOnPowerOff(PsuEventHandler h) { powerOffHandler = h; }

void Psu::setOnError(PsuEventMessageHandler h) { psuState.setOnError(h); }

void Psu::setOnAlert(PsuEventMessageHandler h) { psuState.setOnAlert(h); }

void Psu::powerOn() {
    if (wh_store)
        restoreWh(info.mWh);
    logger->begin();
    startTime = infoUpdated = powerInfoUpdated = lastCheck = millis();
    psuState.clear();
    setState(POWER_ON, true);
    onPowerOn();
}

void Psu::onPowerOn() {
    if (powerOnHandler)
        powerOnHandler();
    active = true;
}

void Psu::powerOff() {
    if (wh_store)
        storeWh(info.mWh);
    logger->end();
    setState(POWER_OFF, true);
    onPowerOff();
}

void Psu::onPowerOff() {
    if (powerOffHandler)
        powerOffHandler();
    active = false;
}

void Psu::setState(PowerState value, bool force) {
    if (!force && !psuState.getPower(value))
        return;
    psuState.setPower(value);
    digitalWrite(POWER_SWITCH_PIN, value);
}

bool Psu::isWhStoreEnabled() { return wh_store; }

float Psu::getOutputVoltage() { return outputVoltage; }

void Psu::setOutputVoltage(float value) {
    outputVoltage = constrain(value, 4, 6);
    mcp4652_write(WRITE_WIPER0_ADDR, quadratic_regression(outputVoltage));
}

bool Psu::begin() {
    float v = config->getValueAsFloat(OUTPUT_VOLTAGE);
    this->setOutputVoltage(v);
    wh_store = false;
    if (config->getValueAsBool(WH_STORE_ENABLED)) {
        if (!restoreWh(info.mWh)) {
            info.mWh = 0;
            wh_store = storeWh(info.mWh);
        }
    }

    PowerState ps;
    switch (config->getValueAsByte(POWER)) {
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

    return true;
}

void Psu::end() { powerOff(); }

void Psu::loop() {
    if (!active)
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
    if (wh_store)
        storeWh(info.mWh);
}

bool Psu::storeWh(double value) {
    return StoreUtils::storeDouble(FILE_VAR_WH, value);
}

bool Psu::restoreWh(double &value) {
    return StoreUtils::restoreDouble(FILE_VAR_WH, value);
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

bool Psu::storeState(PowerState value) {
    return StoreUtils::storeInt(FILE_VAR_POWER_STATE, (byte)value);
}

bool Psu::restoreState(PowerState &value) {
    int buf = 0;
    bool res = StoreUtils::restoreInt(FILE_VAR_POWER_STATE, buf);
    if (res)
        value = PowerState(buf);
    return res;
}

size_t Psu::printDiag(Print *p) {
    size_t n = PrintUtils::print_nameP_value(p, str_power,
                                             getPsuState()->getPowerStateStr());
    n += PrintUtils::print_nameP_value(p, str_output, outputVoltage);
    if (getPsuState()->getPower(POWER_ON)) {
        n += PrintUtils::print_nameP_value(p, str_uptime, getUptime());
    }
    return n;
}
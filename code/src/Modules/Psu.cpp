#include "Modules/PsuModule.h"

#include "ina231.h"
#include "mcp4652.h"

using namespace PrintUtils;
using namespace StrUtils;
using namespace StoreUtils;


bool PsuModule::onInit() {
    setVoltage(config_->getValueAsFloat(OUTPUT_VOLTAGE));
    enableWhStore(config_->getValueAsBool(WH_STORE_ENABLED));
    startTime = infoUpdated = powerInfoUpdated = listenerUpdate_ = lastStore = 0;
    pinMode(POWER_SWITCH_PIN, OUTPUT);
    clearErrorsAndAlerts();
    return true;
}

void PsuModule::powerOn() {
    startTime = infoUpdated = powerInfoUpdated = lastCheck = millis();
    info.mWh = 0;
    if (isWhStoreEnabled())
        restoreWh(info.mWh);
    setState(POWER_ON);
}

void PsuModule::powerOff() {
    if (wh_store)
        storeWh(info.mWh);
    setState(POWER_OFF);
}

void PsuModule::setState(PsuState value) {
    if (state == value)
        return;
    state = value;
    digitalWrite(POWER_SWITCH_PIN, value);
    clearErrorsAndAlerts();
    storeState(state);
    if (stateChangeHandler)
        stateChangeHandler(state, status);
}

bool PsuModule::checkVoltageRange(double value) { return alterRange = value > 7; }

void PsuModule::setVoltage(double value) {
    if (checkVoltageRange(value))
        outputVoltage = constrain(value, 11.8, 12.9);
    else
        outputVoltage = constrain(value, 4.1, 5.3);

    mcp4652_write(WRITE_WIPER0_ADDR, quadratic_regression(value));
}

bool PsuModule::onStart() {
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

void PsuModule::onLoop() {
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
            info.P * (sincePowerReaded / ONE_SECOND_ms) / ONE_HOUR_ms;
        
        info.time = now;
    }

    if (millis_passed(listenerUpdate_, now) >= PSU_LOG_INTERVAL_ms) {
        PsuData pi = getInfo();
        listener_->log(pi);
        listenerUpdate_ = now;
        // TODO
        if (dataHandler_) dataHandler_(pi);
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

void PsuModule::setWh(double value) {
    info.mWh = value;;
    if (wh_store) storeWh(info.mWh);
}

bool PsuModule::storeWh(double value) {
    bool res = storeDouble(FS_WH_VAR, value);
    return res;
}

bool PsuModule::enableWhStore(bool new_value) {
    if (wh_store == new_value)
        return true;
    wh_store = new_value ? restoreWh(info.mWh) : false;
    println_moduleP_nameP_value(out, str_psu, str_store,
                                getEnabledStr(wh_store).c_str());
    return wh_store == new_value;
}

bool PsuModule::restoreWh(double &value) {
    bool res = false;
    if (restoreDouble(FS_WH_VAR, value)) {
        print_ident(out, FPSTR(str_psu));
        println_nameP_value(out, str_restore, value);
        res = true;
    }
    return res;
}

const float PsuModule::getP() { return info.P; }

const float PsuModule::getV() { return info.V; }

const float PsuModule::getI() { return info.I; }

const double PsuModule::getWh() { return info.mWh / ONE_WATT_mW; }

const unsigned long PsuModule::getUptime() {
    return millis_passed(startTime, infoUpdated) / ONE_SECOND_ms;
}

bool PsuModule::isWhStoreEnabled(void) {
    wh_store = config_->getValueAsBool(WH_STORE_ENABLED);
    return wh_store;
}

bool PsuModule::storeState(PsuState value) {
    return StoreUtils::storeInt(FS_POWER_STATE_VAR, (byte) value);
}

bool PsuModule::restoreState(PsuState &value) {
    byte buf;
    bool res = StoreUtils::restoreByte(FS_POWER_STATE_VAR, buf);
    if (res) value = (PsuState) buf;
    return res;
}

size_t PsuModule::onDiag(Print *p) {
    DynamicJsonDocument doc(256);

    doc[FPSTR(str_power)] = getStateStr(state);
    doc[FPSTR(str_status)] = getStatusStr(status);
    doc[FPSTR(str_output)] = outputVoltage;
    doc[FPSTR(str_store)] = getBoolStr(wh_store);
    doc[FPSTR(str_mod)] =  getBoolStr(alterRange);
    doc[FPSTR(str_uptime)] = getUptime();
    doc[FPSTR(str_data)] = info.toString();
    
    return serializeJsonPretty(doc, *p);
}

void PsuModule::togglePower() {
    if (checkState(POWER_ON))
        powerOff();
    else
        powerOn();
}

PsuStatus PsuModule::getStatus(void) { return status; }

PsuState PsuModule::getState(void) { return state; }

PsuError PsuModule::getError(void) { return error; }

PsuAlert PsuModule::getAlert(void) { return alert; }

bool PsuModule::checkState(PsuState value) { return state == value; }

bool PsuModule::checkStatus(PsuStatus value) { return status == value; }

void PsuModule::setOnStateChange(PsuStateChangeHandler h) { stateChangeHandler = h; };

void PsuModule::setOnData(PsuDataHandler h) { dataHandler_ = h; };

void PsuModule::setError(PsuError value) {
    error = value;
    setStatus(PSU_ERROR);
}

void PsuModule::setAlert(PsuAlert value) {
    alert = value;
    setStatus(PSU_ALERT);
}

void PsuModule::setStatus(PsuStatus value) {
    if (status == value)
        return;
    status = value;
    if (stateChangeHandler)
        stateChangeHandler(state, status);
}

const PsuData PsuModule::getInfo() { return info; }

const float PsuModule::getVoltage() { return outputVoltage; }

void PsuModule::clearErrorsAndAlerts() {
    alert = PSU_ALERT_NONE;
    error = PSU_ERROR_NONE;
    setStatus(PSU_OK);
}
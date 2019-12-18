#include "Modules/Psu.h"

#include "Hardware/mcp4652.h"
#include "Hardware/ina231.h"

using namespace PrintUtils;
using namespace StrUtils;
using namespace FSUtils;

namespace Modules {

bool Psu::onInit() {
    clearErrorsAndAlerts();
    return true;
}

bool Psu::onStart() {
    setVoltage(config_->getValueAsFloat(OUTPUT_VOLTAGE));
    enableWhStore(config_->getValueAsBool(WH_STORE_ENABLED));
    PsuState ps = POWER_OFF;
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
        break;
    }
    pinMode(POWER_SWITCH_PIN, OUTPUT);
    ina231_configure();
    mcp4652_init();
    if (ps == POWER_ON)
        powerOn();
    else
        powerOff();
    return true;
}

void Psu::powerOn() {
    startTime_ = infoUpdated_ = powerInfoUpdated_ = lastCheck_ = millis();
    info_.Wh = 0;
    if (isWhStoreEnabled())
        restoreWh(info_.Wh);
    applyState(POWER_ON);
}

void Psu::powerOff() {
    if (wh_store_)
        storeWh(info_.Wh);
    applyState(POWER_OFF);
}

void Psu::applyState(PsuState value) {
    if (state_ == value)
        return;
    state_ = value;
    digitalWrite(POWER_SWITCH_PIN, value);
    clearErrorsAndAlerts();
    storeState(state_);
    if (stateChangeHandler_)
        stateChangeHandler_(state_, status_);
}

bool Psu::checkVoltageRange(double value) { return alterRange_ = value > 7; }

void Psu::setVoltage(double value) {
    if (checkVoltageRange(value))
        outputVoltage_ = constrain(value, 11.8, 12.9);
    else
        outputVoltage_ = constrain(value, 4.1, 5.3);

    mcp4652_write(WRITE_WIPER0_ADDR, quadratic_regression(value, alterRange_));
}

void Psu::onLoop() {
    if (!checkState(POWER_ON))
        return;

    unsigned long now = millis();
    unsigned long passed  = millis_passed(infoUpdated_, now);
    if (passed >= MEASUREMENT_INTERVAL_ms) {
        infoUpdated_ = now;
        info_.V = ina231_read_voltage(); 
        info_.I = ina231_read_current();
        info_.P = ina231_read_power();
        double intervalInSeconds = (double) passed / 1000;
        double Ws = intervalInSeconds * info_.P;
        info_.Wh += (Ws / 3600);
        info_.time = now;        
    }

    if (millis_passed(listenerUpdate_, now) >= PSU_LOG_INTERVAL_ms) {
        PsuData pi = getInfo();
        listener_->onPsuData(pi);
        listenerUpdate_ = now;
        // TODO
        if (dataHandler_) dataHandler_(pi);
    }

    if (millis_passed(lastCheck_, now) > PSU_CHECK_INTERVAL_s * ONE_SECOND_ms) {
        if (info_.V < PSU_VOLTAGE_LOW_v) {
            setError(PSU_ERROR_LOW_VOLTAGE);
        } else if (info_.I <= PSU_LOAD_LOW_a) {
            setAlert(PSU_ALERT_LOAD_LOW);
        } else {
            clearErrorsAndAlerts();
        }
        lastCheck_ = now;
    }

    if (millis_passed(lastStore_, now) > ONE_MINUTE_ms) {
        if (wh_store_)
            storeWh(info_.Wh);
        lastStore_ = now;
    }
}

void Psu::setWh(double value) {
    info_.Wh = value;
    if (wh_store_) storeWh(info_.Wh);
}

bool Psu::storeWh(double value) {
    bool res = FSUtils::writeDouble(FS_WH_VAR, value);
    return res;
}

bool Psu::enableWhStore(bool enabled) {
    if (wh_store_ == enabled) return true;
   
    if (enabled)
        wh_store_ = restoreWh(info_.Wh);
    else 
        wh_store_ = false;

    PrintUtils::print_ident(out_, FPSTR(str_psu));
    PrintUtils::print(out_, FPSTR(str_store), getEnabledStr(wh_store_));
    PrintUtils::println(out_);
    return wh_store_ == enabled;
}

bool Psu::restoreWh(double &value) {
    bool res = false;    
    if (FSUtils::readDouble(FS_WH_VAR, value)) {
        PrintUtils::print_ident(out_, FPSTR(str_psu));
        PrintUtils::print(out_, FPSTR(str_total), value);
        PrintUtils::println(out_);
        res = true;
    }
    return res;
}

const float Psu::getP() { return info_.P; }

const float Psu::getV() { return info_.V; }

const float Psu::getI() { return info_.I; }

const double Psu::getWh() { return info_.Wh / ONE_WATT_mW; }

const unsigned long Psu::getPsuUptime() {
    return millis_passed(startTime_, infoUpdated_) / ONE_SECOND_ms;
}

bool Psu::isWhStoreEnabled(void) {
    wh_store_ = config_->getValueAsBool(WH_STORE_ENABLED);
    return wh_store_;
}

bool Psu::storeState(PsuState value) {
    bool res = FSUtils::writeInt(FS_POWER_STATE_VAR, (long) value);
    PrintUtils::print_ident(out_, FPSTR(str_psu));
    if (res) {    
        PrintUtils::print(out_, FPSTR(str_stored), value);
    } else {
        PrintUtils::print(out_, FPSTR(str_store), FPSTR(str_failed));        
    }
    PrintUtils::println(out_);
    return res;
}

bool Psu::restoreState(PsuState &value) {
    long buf;
    bool res = FSUtils::readInt(FS_POWER_STATE_VAR, buf);
    PrintUtils::print_ident(out_, FPSTR(str_psu));
    if (res) {    
        value = (PsuState) buf;
        PrintUtils::print(out_, FPSTR(str_restored), value);
    }  else  {
        PrintUtils::print(out_, FPSTR(str_restore), FPSTR(str_failed));        
    }
    PrintUtils::println(out_);
    return res;
}

void Psu::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_power)] = getPsuStateStr(state_);
    if (checkState(POWER_ON)) {
        doc[FPSTR(str_time)] = getPsuUptime();
    }
    doc[FPSTR(str_status)] = getStatusStr(status_);
    doc[FPSTR(str_output)] = outputVoltage_;
    doc[FPSTR(str_mod)] =  getBoolStr(alterRange_);

    doc[FPSTR(str_data)] = info_.toString();   
}

void Psu::togglePower() {
    if (checkState(POWER_ON))
        powerOff();
    else
        powerOn();
}

PsuStatus Psu::getStatus(void) { return status_; }

PsuState Psu::getState(void) { return state_; }

PsuError Psu::getError(void) { return error_; }

PsuAlert Psu::getAlert(void) { return alert_; }

bool Psu::checkState(PsuState value) { return state_ == value; }

bool Psu::checkStatus(PsuStatus value) { return status_ == value; }

void Psu::setOnStateChange(PsuStateChangeHandler h) { stateChangeHandler_ = h; };

void Psu::setOnData(PsuDataHandler h) { dataHandler_ = h; };

void Psu::setError(PsuError value) {
    error_ = value;
    setStatus(PSU_ERROR);
}

void Psu::setAlert(PsuAlert value) {
    alert_ = value;
    setStatus(PSU_ALERT);
}

void Psu::setStatus(PsuStatus value) {
    if (status_ == value) return;
    status_ = value;
    if (stateChangeHandler_)
        stateChangeHandler_(state_, status_);
}

const PsuData Psu::getInfo() { return info_; }

const float Psu::getVoltage() { return outputVoltage_; }

void Psu::clearErrorsAndAlerts() {
    alert_ = PSU_ALERT_NONE;
    error_ = PSU_ERROR_NONE;
    status_ = PSU_OK;
}

}
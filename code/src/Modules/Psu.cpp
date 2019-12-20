#include "Modules/Psu.h"

#include "Hardware/mcp4652.h"
#include "Hardware/ina231.h"

using namespace PrintUtils;
using namespace StrUtils;
using namespace FSUtils;

namespace Modules {

bool Psu::onInit() {
    ina231_configure();
    clearErrorsAndAlerts();
    return true;
}

bool Psu::onStart() {
    pinMode(POWER_SWITCH_PIN, OUTPUT);
    switch (getBootPowerState()) {
        case BOOT_POWER_OFF:
            powerOff();
            break;
        case BOOT_POWER_ON:
            powerOn();
            break;
        case BOOT_POWER_LAST_STATE:
            PsuState stored = POWER_OFF;
            restoreState(stored);
            if (stored == POWER_ON)
                powerOn();
             else
                powerOff();
            break;
        }
    return true;
}

void Psu::onLoop() {
    if (state_ != POWER_ON) return;

    unsigned long now = millis();
    unsigned long passed = millis_passed(infoUpdated_, now);
    if (passed >= MEASUREMENT_INTERVAL_ms) {
        infoUpdated_ = now;
        info_.V = ina231_read_voltage();
        info_.I = ina231_read_current();
        info_.P = ina231_read_power();
        double sec = (double) passed / 1000;
        double ws = sec * info_.P;
        info_.Wh += (ws / 3600);
        info_.time = now;
    }

    if (millis_passed(listenerUpdate_, now) >= PSU_LOG_INTERVAL_ms) {
        if (dataListener_) dataListener_->onPsuData(info_);
        listenerUpdate_ = now;
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
        if (isWhStoreEnabled()) storeWh(info_.Wh);
        lastStore_ = now;
    }
}

void Psu::powerOn() {
    startTime_ = infoUpdated_ = powerInfoUpdated_ = lastCheck_ = millis();
    if (isWhStoreEnabled()) restoreWh(info_.Wh);
    if (isStateStoreEnabled()) storeState(POWER_ON);
    float voltage = getOutputVoltage();
    setOutputVoltage(voltage);

    applyState(POWER_ON);
}

void Psu::powerOff() {
    if (isWhStoreEnabled()) storeWh(info_.Wh);
    if (isStateStoreEnabled()) storeState(POWER_OFF);
    
    applyState(POWER_OFF);
}

void Psu::applyState(PsuState value) {
    if (state_ == value) return;
    state_ = value;
    digitalWrite(POWER_SWITCH_PIN, value);
    if (stateChangeHandler_)
        stateChangeHandler_(state_);
}

int Psu::mapVoltage(const float value) {
    float v;
    float min;
    float max;
    if (value > 6) {
        min = 11.83;
        max = 12.9;
    } else {
        min = 4.08;
        max = 5.30;
    }
    v = constrain(value, min, max);
    return quadratic_regression(v, min);
}

bool Psu::isPowerOn() const {
    return state_ == POWER_ON;
}

bool Psu::isWhStoreEnabled() const {
    return config_->getValueAsBool(WH_STORE_ENABLED);
}

void Psu::setOutputVoltage(const float value) {
    PrintUtils::print_ident(out_, FPSTR(str_psu));
    PrintUtils::print(out_, FPSTR(str_voltage), FPSTR(str_arrow_dest), value);
    PrintUtils::println(out_);
    applyVoltage(value);
}

void Psu::applyVoltage(const float value) {
    uint8_t v = mapVoltage(value);
    mcp4652_set(v);
}

float Psu::getOutputVoltage() const {
    return config_->getValueAsFloat(OUTPUT_VOLTAGE);
}

BootPowerState Psu::getBootPowerState() const {
    return (BootPowerState)config_->getValueAsByte(POWER);
}

bool Psu::isStateStoreEnabled() const {
    return getBootPowerState() == BOOT_POWER_LAST_STATE;
}

void Psu::setWh(double value) {
    info_.Wh = value;
    if (isWhStoreEnabled()) storeWh(info_.Wh);
}

bool Psu::storeWh(double value) {
    PrintUtils::print_ident(out_, FPSTR(str_psu));
    bool res = FSUtils::writeDouble(FS_WH_VAR, value);
    if (res) {
        PrintUtils::print(out_, FPSTR(str_total), FPSTR(str_arrow_dest),  String(value, 6));
    } else {
        PrintUtils::print(out_, FPSTR(str_failed));
    }
    PrintUtils::println(out_);
    return res;
}

bool Psu::restoreWh(double& value) {
    PrintUtils::print_ident(out_, FPSTR(str_psu));
    bool res = FSUtils::readDouble(FS_WH_VAR, value);
    if (res) {
        PrintUtils::print(out_, FPSTR(str_total), FPSTR(str_arrow_src), String(value, 6));
    } else {
        PrintUtils::print(out_, FPSTR(str_failed));
    }
    PrintUtils::println(out_);
    return res;
}

const unsigned long Psu::getUptime() const {
    return millis_passed(startTime_, infoUpdated_) / ONE_SECOND_ms;
}

bool Psu::storeState(PsuState value) {
    bool res = FSUtils::writeInt(FS_POWER_STATE_VAR, (long)value);
    PrintUtils::print_ident(out_, FPSTR(str_psu));
    if (res) {
        PrintUtils::print(out_, FPSTR(str_stored), value);
    } else {
        PrintUtils::print(out_, FPSTR(str_store), FPSTR(str_failed));
    }
    PrintUtils::println(out_);
    return res;
}

bool Psu::restoreState(PsuState& value) {
    PrintUtils::print_ident(out_, FPSTR(str_psu));
    long buf;
    bool res = FSUtils::readInt(FS_POWER_STATE_VAR, buf);
    if (res) {
        value = (PsuState) buf;
        PrintUtils::print(out_, FPSTR(str_restored), value);
    } else {
        PrintUtils::print(out_, FPSTR(str_restore), FPSTR(str_failed));
    }
    PrintUtils::println(out_);
    return res;
}

void Psu::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_power)] = getPsuStateStr(state_);
    if (state_ == POWER_ON) {
        doc[FPSTR(str_time)] = getUptime();
    }
    doc[FPSTR(str_status)] = getStatusStr(status_);
    doc[FPSTR(str_data)] = info_.toString();
}

void Psu::togglePower() {
    if (state_ == POWER_ON)
        powerOff();
    else
        powerOn();
}

PsuStatus Psu::getStatus(void) const { return status_; }

PsuState Psu::getState(void) const { return state_; }

PsuError Psu::getError(void) const { return error_; }

PsuAlert Psu::getAlert(void) const { return alert_; }

void Psu::setOnStateChange(PsuStateChangeHandler h) {
    stateChangeHandler_ = h;
};

void Psu::setOnStatusChange(PsuStatusChangeHandler h) {
    statusChangeHandler_ = h;
};

void Psu::setOnData(PsuDataListener* l) {
    dataListener_ = l;
}

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
    if (state_ == POWER_ON)
        if (statusChangeHandler_) statusChangeHandler_(status_);
}

const PsuData Psu::getInfo() const {
    return info_;
}

void Psu::clearErrorsAndAlerts() {
    alert_ = PSU_ALERT_NONE;
    error_ = PSU_ERROR_NONE;
    status_ = PSU_OK;
}

}  // namespace Modules
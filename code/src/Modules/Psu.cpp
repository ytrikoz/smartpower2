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
    if (isWhStoreEnabled()) restoreWh(info_.Wh);
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
        double sec = (double)passed / 1000;
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
        if (isWhStoreEnabled() && info_.Wh != lastStoredWh_) storeWh(info_.Wh);
        lastStore_ = now;
    }    
}

bool Psu::onConfigChange(const ConfigItem param, const String& value) {
    if (state_ == POWER_ON) {
        if (param == OUTPUT_VOLTAGE) {
            setOutputVoltage(value.toFloat());
        }
        if (param == BOOT_POWER && value.toInt() == BOOT_POWER_LAST_STATE) {
            storeState(POWER_ON);
        }
    }
    return true;
}

void Psu::onExecute(const String &param, const String &value) {
    mcp4652_set(param.toInt());
}

void Psu::powerOn() {
    digitalWrite(POWER_SWITCH_PIN, mapState(POWER_ON));
    if (state_ == POWER_ON) return;
    startTime_ = infoUpdated_ = powerInfoUpdated_ = lastCheck_ = millis();
    state_ = POWER_ON;    
    if (isStateStoreEnabled() && state_ != lastStoredState_) storeState(POWER_ON);
    float voltage = getOutputVoltage();
    setOutputVoltage(voltage);
    onStateChangeEvent(state_);
}

void Psu::powerOff() {
    digitalWrite(POWER_SWITCH_PIN, mapState(POWER_OFF));
    if (state_ == POWER_OFF) return;
    state_ = POWER_OFF;    
    if (isWhStoreEnabled() && lastStoredWh_ != info_.Wh) storeWh(info_.Wh);
    if (isStateStoreEnabled() && state_!= lastStore_) storeState(POWER_OFF);
    onStateChangeEvent(state_);
}

void Psu::onStateChangeEvent(PsuState value) {
    if (stateChangeHandler_)
        stateChangeHandler_(state_);
}

bool Psu::mapState(const PsuState state) {
    return state == POWER_ON ? 0 : 1;
}

uint8_t Psu::mapVoltage(const float value) {

    float min;
    float max;
    if (value > 6) {
        min = 11.83f;
        max = 12.9f;
    } else {
        min = 4.03f;
        max = 5.30f;
    }
    float v;
    v = constrain(value, min, max);
    return quadratic_regression(v, min);
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
    return config_->asFloat(OUTPUT_VOLTAGE);
}

bool Psu::isPowerOn() const {
    return state_ == POWER_ON;
}

bool Psu::isWhStoreEnabled() const {
    return config_->asBool(WH_STORE_ENABLED);
}

BootPowerState Psu::getBootPowerState() const {
    return (BootPowerState)config_->asByte(BOOT_POWER);
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
        PrintUtils::print(out_, FPSTR(str_arrow_dest), String(value, 6));
        lastStoredWh_ = value;
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
        PrintUtils::print(out_, FPSTR(str_arrow_src), String(value, 6));
    } else {
        PrintUtils::print(out_, FPSTR(str_failed));
    }
    PrintUtils::println(out_);
    return res;
}

bool Psu::storeState(PsuState value) {
    PrintUtils::print_ident(out_, FPSTR(str_psu));
    PrintUtils::print(out_, FPSTR(str_state), FPSTR(str_arrow_dest));
    bool res = FSUtils::writeInt(FS_POWER_STATE_VAR, (long) value);
    if (res) {
        lastStoredState_ = value;
        PrintUtils::print(out_, getPsuStateStr(value));
    } else {
        PrintUtils::print(out_, FPSTR(str_failed));
    }
    PrintUtils::println(out_);
    return res;
}

bool Psu::restoreState(PsuState& value) {
    PrintUtils::print_ident(out_, FPSTR(str_psu));
    PrintUtils::print(out_, FPSTR(str_state), FPSTR(str_arrow_src));
    long buf;
    bool res = false;
    if (FSUtils::readInt(FS_POWER_STATE_VAR, buf)) {
        if (buf >= 0 && buf <= 1) {
            value = (PsuState)buf;
            PrintUtils::print(out_, getPsuStateStr(value));
            lastStoredState_ = value;
            res = true;
        } else {
            PrintUtils::print(out_, FPSTR(str_invalid));
        }
    } else {
        PrintUtils::print(out_, FPSTR(str_failed));
    }
    PrintUtils::println(out_);
    return res;
}

void Psu::onDiag(const JsonObject& doc) {
    doc[FPSTR(str_power)] = getPsuStateStr(state_);
    if (state_ == POWER_ON) {
        doc[FPSTR(str_time)] = TimeUtils::format_elapsed_short(getUptime());
        doc[FPSTR(str_status)] = getStatusStr(status_);
        doc[FPSTR(str_data)] = info_.toJson();
    }
}

void Psu::togglePower() {
    if (state_ == POWER_ON)
        powerOff();
    else
        powerOn();
}

const unsigned long Psu::getUptime() const {
    return millis_passed(startTime_, infoUpdated_);
}

PsuStatus Psu::getStatus(void) const { return status_; }

PsuState Psu::getState(void) const { return state_; }

PsuError Psu::getError(void) const { return error_; }

PsuAlert Psu::getAlert(void) const { return alert_; }

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

void Psu::setOnStateChange(PsuStateChangeHandler h) {
    stateChangeHandler_ = h;
};

void Psu::setOnStatusChange(PsuStatusChangeHandler h) {
    statusChangeHandler_ = h;
};

void Psu::setOnData(PsuDataListener* l) {
    dataListener_ = l;
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
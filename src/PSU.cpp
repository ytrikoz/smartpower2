#include "Psu.h"

Psu::Psu() {
    info = PsuInfo();

    wattHoursCalculationEnabled = false;

    active = false;
    initialized = false;

    started_ms = 0;
    updated_ms = 0;
    statsUpdated_ms = 0;

    pinMode(POWER_SWITCH_PIN, OUTPUT);
}

PsuInfo Psu::getInfo() {
    info.time = this->updated_ms;
    info.current = getCurrent();
    info.power = getPower();
    info.voltage = getVoltage();
    return info;
}

void Psu::init() {
    // meter
    ina231_configure();
    // pot
    mcp4652_init();

    initialized = true;
}

void Psu::togglePower() { setState(state == POWER_OFF ? POWER_ON : POWER_OFF); }

void Psu::setConfig(ConfigHelper *config) { this->config = config; }

void Psu::setOnPowerOn(PsuEventHandler h) { onPowerOn = h; }

void Psu::setOnPowerOff(PsuEventHandler h) { onPowerOff = h; }

void Psu::setState(PowerState value, bool forceUpdate) {
    output->print(FPSTR(str_psu));
    output->printf_P(strf_power, value == POWER_ON ? "on" : "off");
    output->println();

    if ((forceUpdate) || (state != value)) {
        state = value;
        if (state == POWER_ON) {
            startMeasure();
            if (onPowerOn) onPowerOn();
        } else if (state == POWER_OFF) {
            endMeasure();
            if (onPowerOff) onPowerOff();
        }
        digitalWrite(POWER_SWITCH_PIN, state);
        storePowerState(state);
    }
}

PowerState Psu::getState() { return state; }

float Psu::getOutputVoltage() { return outputVoltage; }

void Psu::setOutputVoltage(float value) {
    output->print(FPSTR(str_psu));
    output->printf_P(strf_output_voltage, value);
    output->println();

    this->outputVoltage = value;
    mcp4652_write(WRITE_WIPER0, quadraticRegression(value));
}

void Psu::begin() {
    if (!initialized) init();

    float v = config->getOutputVoltage();
    this->setOutputVoltage(v);

    PowerState ps;
    switch (config->getBootPowerState()) {
        case BOOT_POWER_OFF:
            ps = POWER_OFF;
            break;
        case BOOT_POWER_ON:
            ps = POWER_ON;
            break;
        case BOOT_POWER_LAST_STATE:
            ps = restorePowerState();
            break;
        default:
            ps = POWER_OFF;
    }

    setState(ps, true);
}

void Psu::storePowerState(PowerState state) {
    File f = SPIFFS.open(FILE_LAST_POWER_STATE, "w");
    f.println(state);
    f.flush();
    f.close();
}

PowerState Psu::restorePowerState() {
    if (!SPIFFS.exists(FILE_LAST_POWER_STATE)) {
        storePowerState(POWER_OFF);
        return POWER_OFF;
    }
    File f = SPIFFS.open(FILE_LAST_POWER_STATE, "r");
    PowerState value = PowerState(f.readString().toInt());
    f.close();
    return value;
}

void Psu::startMeasure() {
    started_ms = millis();
    updated_ms = started_ms;
    statsUpdated_ms = started_ms;
    active = true;
}
void Psu::endMeasure() { active = false; }

void Psu::loop() {
    if (!active) return;
    unsigned long now_ms = millis();
    if (now_ms - updated_ms >= MEASUREMENT_INTERVAL_ms) {
        info.voltage = ina231_read_voltage();
        info.current = ina231_read_current();
        updated_ms = now_ms;
    }
    if (now_ms - statsUpdated_ms >= ONE_SECOND_ms) {
        info.power = ina231_read_power();
        if (wattHoursCalculationEnabled) {
            info.wattSeconds += info.power;
        }
        statsUpdated_ms = now_ms;
    }
}

void Psu::setWattHours(double value) {
    this->info.wattSeconds = value / ONE_HOUR_s;
}

void Psu::enableWattHoursCalculation(bool enabled) {
    this->wattHoursCalculationEnabled = enabled;
}

bool Psu::isWattHoursCalculationEnabled() {
    return this->wattHoursCalculationEnabled;
}

float Psu::getVoltage() { return this->info.voltage; }

float Psu::getCurrent() { return this->info.current; }

float Psu::getPower() { return this->info.power; }

double Psu::getWattHours() { return this->info.wattSeconds * ONE_HOUR_s; }

String Psu::toString() {
    String str = String(getVoltage(), 3);
    str += ",";
    str += String(getCurrent(), 3);
    str += ",";
    str += String(getPower(), 3);
    str += ",";
    str += String(getWattHours(), 3);
    return str;
}

unsigned long Psu::getDuration_s() {
    return (updated_ms - started_ms) / ONE_SECOND_ms;
}

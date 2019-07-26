#include "PSU.h"

PSU::PSU() {
    voltage = 0;
    power = 0;
    current = 0;
    wattSeconds = 0;
    wattHoursCalculationEnabled = false;
    started_ms = 0;
    finished_ms = 0;
    active = false;
    initialized = false;
}

void PSU::init() {
    // meter
    ina231_configure();
    // pot
    mcp4652_init();
    
    initialized = true;
}

void PSU::begin() {     
    if (!initialized) return;

    started_ms = millis();
    updated_ms = started_ms - MEASUREMENT_INTERVAL_ms;
    finished_ms = started_ms;
    active = true; 
}

void PSU::end() { active = false; }

void PSU::loop() {
    if (!active) return;

    unsigned long now_ms = millis();    
    
    if (now_ms - updated_ms >= MEASUREMENT_INTERVAL_ms) {
        voltage = ina231_read_voltage();        
        current = ina231_read_current();                    
        updated_ms = millis();
    }

    if (now_ms - finished_ms >= ONE_SECOND_ms) {
        finished_ms += ONE_SECOND_ms;
        power = ina231_read_power();
        if (wattHoursCalculationEnabled) {
            wattSeconds += power; 
        }       
    }
}

void PSU::setWattHours(double value) {
    this->wattSeconds = value / ONE_HOUR_s;
}

void PSU::enableWattHoursCalculation(bool enabled) {
    wattHoursCalculationEnabled = enabled;
}

bool PSU::isWattHoursCalculationEnabled() { return wattHoursCalculationEnabled; }

float PSU::getVoltage() { return voltage; }

float PSU::getCurrent() { return current; }

float PSU::getPower() { return power; }

double PSU::getWattHours() {
    return wattSeconds * ONE_HOUR_s;
}

String PSU::toString() {
    String str = String(getVoltage(), 3);
    str += ",";
    str += String(getCurrent(), 3);
    str += ",";
    str += String(getPower(), 3);
    str += ",";
    str += String(getWattHours(), 3);
    return str;
}

unsigned long PSU::getDuration_s()
{
    return (finished_ms - started_ms) / ONE_SECOND_ms;
}


#include "Multimeter.h"

#include "consts.h"

Multimeter::Multimeter() {
    mcp4652_init();
    voltage = 0;
    power = 0;
    current = 0;
    wattSeconds = 0;
    wattHoursCalculationEnabled = false;
    started_ms = 0;
    finished_ms = 0;
    active = false;
}

void Multimeter::begin() {     
    started_ms = millis();
    updated_ms = started_ms - MEASUREMENT_INTERVAL_ms;
    finished_ms = started_ms;
    active = true; 
}

void Multimeter::end() { active = false; }

void Multimeter::loop() {
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

void Multimeter::setWattHours(double value) {
    this->wattSeconds = value / ONE_HOUR_s;
}

void Multimeter::enableWattHoursCalculation(bool enabled) {
    wattHoursCalculationEnabled = enabled;
}

bool Multimeter::isWattHoursCalculationEnabled() { return wattHoursCalculationEnabled; }

float Multimeter::getVoltage() { return voltage; }

float Multimeter::getCurrent() { return current; }

float Multimeter::getPower() { return power; }

double Multimeter::getWattHours() {
    return wattSeconds * ONE_HOUR_s;
}

unsigned long Multimeter::getDuration_s()
{
    return (finished_ms - started_ms) / ONE_SECOND_ms;
}


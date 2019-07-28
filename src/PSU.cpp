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
    
    pinMode(POWER_SWITCH_PIN, OUTPUT);
}

void PSU::init() {
    // meter
    ina231_configure();
    // pot
    mcp4652_init();

    initialized = true;   
}

void PSU::togglePower()
{
    setState(state == POWER_OFF ? POWER_ON : POWER_OFF);
}

void PSU::setConfig(ConfigHelper *config)
{
    this->config = config;
}

void PSU::setOnPowerOn(PSUEventHandler h)
{
    onPowerOn = h;
}

void PSU::setOnPowerOff(PSUEventHandler h)
{
    onPowerOff = h;
}

void PSU::setState(PowerState value, bool forceUpdate) {
    output->print(FPSTR(str_psu));
    output->println(value == POWER_ON ? "on" : "off");
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
        setLastPowerState(state);
    }       
}

PowerState PSU::getState() { return state; }

float PSU::getOutputVoltage()
{
    return outputVoltage;
}

void PSU::setOutputVoltage(float value)
{
    output->print(FPSTR(str_psu));
    output->printf_P(strf_output_voltage, value);
    
    outputVoltage = value;    
    mcp4652_write(WRITE_WIPER0, quadraticRegression(value));
    
    output->println();
}

void PSU::begin() {     
    if (!initialized) {
        init();
    };
    float outputVoltage = config->getOutputVoltage();
    this->setOutputVoltage(outputVoltage);
    
    PowerState state;
    switch (config->getBootPowerState()) {
    case BOOT_POWER_OFF:
        state = POWER_OFF;
        break;
    case BOOT_POWER_ON:
        state = POWER_ON;
        break;
    case BOOT_POWER_LAST_STATE:
        state = getLastPowerState();
        break;
    default:
        state = POWER_OFF;
    }

    setState(state, true);
}

void PSU::setLastPowerState(PowerState state) {
    File f = SPIFFS.open(FILE_LAST_POWER_STATE, "w");
    f.println(state);
    f.flush();
    f.close();
}

PowerState PSU::getLastPowerState() {
    if (!SPIFFS.exists(FILE_LAST_POWER_STATE)) {
        File f = SPIFFS.open(FILE_LAST_POWER_STATE, "w");
        f.println(POWER_OFF);
        f.flush();
        f.close();
        return POWER_OFF;
    }
    File f = SPIFFS.open(FILE_LAST_POWER_STATE, "r");
    PowerState value = PowerState(f.readString().toInt());
    f.close();
    return value;
}

void PSU::startMeasure() {
    started_ms = millis();
    updated_ms = started_ms - MEASUREMENT_INTERVAL_ms;
    finished_ms = started_ms;
    active = true; 
}
void PSU::endMeasure() {
     active = false; 
     
}

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
    this->wattHoursCalculationEnabled = enabled;
}

bool PSU::isWattHoursCalculationEnabled() { return this->wattHoursCalculationEnabled; }

float PSU::getVoltage() { return this->voltage; }

float PSU::getCurrent() { return this->current; }

float PSU::getPower() { return this->power; }

double PSU::getWattHours() {
    return this->wattSeconds * ONE_HOUR_s;
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


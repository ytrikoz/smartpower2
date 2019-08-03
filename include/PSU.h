#pragma once
#include <Arduino.h>
#include <mcp4652.h>

#include "consts.h"
#include "Types.h"

#include "ConfigHelper.h"

#define POWER_SWITCH_PIN D6

typedef std::function<void()> PsuEventHandler;

class Psu : public PsuInfoProvider {
   public:
    Psu();
    void init();
    void begin();
    void startMeasure();
    void endMeasure();
    void loop();

    void togglePower();
    void setState(PowerState value, bool forceUpdate = false);
    PowerState getState();
    void setConfig(ConfigHelper*);
    void setOnPowerOn(PsuEventHandler);
    void setOnPowerOff(PsuEventHandler);
    void setOutputVoltage(float voltage);
    float getOutputVoltage();

    String toString();
    float getVoltage();
    float getCurrent();
    float getPower();
    double getWattHours();
    unsigned long getDuration_s();
    void setWattHours(double value);
    void enableWattHoursCalculation(bool enabled);
    bool isWattHoursCalculationEnabled();
    
    PsuInfo getInfo();
   private:
    void storePowerState(PowerState state);
    PowerState restorePowerState();
    unsigned long updated_ms, statsUpdated_ms;
    unsigned long started_ms;
    bool active;
    bool initialized;
    bool wattHoursCalculationEnabled;

    PsuInfo info;
    ConfigHelper* config;

    PowerState state;
    float outputVoltage;
    Print *output = &USE_SERIAL;
    PsuEventHandler onPowerOn, onPowerOff;

    static int quadraticRegression(double volt) {
        double a = 0.0000006562;
        double b = 0.0022084236;
        float c = 4.08;
        double d = b * b - a * (c - volt);
        double root = (-b + sqrt(d)) / a;
        if (root < 0)
            root = 0;
        else if (root > 255)
            root = 255;
        return root;
    }    
};

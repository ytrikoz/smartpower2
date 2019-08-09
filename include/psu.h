#pragma once
#include <Arduino.h>
#include <mcp4652.h>

#include "time_utils.h"
#include "consts.h"
#include "types.h"

#include "ConfigHelper.h"

#define POWER_SWITCH_PIN D6

typedef std::function<void()> PsuEventHandler;

class Psu : public PsuInfoProvider {
   public:
    Psu();
    void begin();
    void start();
    void stop();
    void loop();

    void togglePower();
    void setState(PowerState value, bool forceUpdate = false);
    void setConfig(ConfigHelper*);
    
    void setOnPowerOn(PsuEventHandler);    
    void setOnPowerOff(PsuEventHandler);

    void setOutputVoltage(float voltage);
    float getOutputVoltage();
    
    PsuInfo getInfo();
    String toString();
    PowerState getState();
    String getStateDescription();
    float getVoltage();
    float getCurrent();
    float getPower();
    double getWattHours();
    unsigned long getDuration();
    void setWattHours(double value);
    void enableWattHoursCalculation(bool enabled);
    bool isWattHoursCalculationEnabled();

   private:
    void init();
    void storePowerState(PowerState state);
    PowerState restorePowerState();
    unsigned long startedAt, updatedAt, calcedAt;
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

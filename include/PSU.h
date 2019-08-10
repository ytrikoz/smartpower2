#pragma once

#include "CommonTypes.h"
#include "ConfigHelper.h"

#define POWER_SWITCH_PIN D6

enum PsuStatus {PSU_OK, PSU_ERROR_LOW_VOLTAGE};

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
    void setOnPowerError(PsuEventHandler);

    void setOutputVoltage(float voltage);
    float getOutputVoltage();

    PsuInfo getInfo();
    String toString();
    PowerState getState();
    String getStateStr();    
    float getVoltage();
    float getCurrent();
    float getPower();
    double getWattHours();
    unsigned long getDuration();
    void setWattHours(double value);
    void enableWattHoursCalculation(bool enabled);
    bool isWattHoursCalculationEnabled();
    
    void printDiag(Print* p);
   private:
    void clearError();
    void error(PsuStatus err);
    void init();
    void storePowerState(PowerState state);
    PowerState restorePowerState();
    PsuStatus status;
    unsigned long startedAt, updatedAt, calcedAt;
    bool active;
    bool initialized;
    bool wattHoursCalculationEnabled;

    PsuInfo info;
    ConfigHelper* config;

    PowerState state;
    
    float outputVoltage;
    Print* output = &USE_SERIAL;
    PsuEventHandler onPowerOn, onPowerOff, onPowerError;

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

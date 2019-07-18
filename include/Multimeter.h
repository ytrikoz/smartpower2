#pragma once
#include <Arduino.h>
#include <mcp4652.h>

#include "types.h"

class Multimeter {
   public:
    Multimeter();

    void begin();
    void end();
    void loop();

    String toString();
    float getVoltage();
    float getCurrent();
    float getPower();
    double getWattHours();
    unsigned long getDuration_s();
    void setWattHours(double value);
    void enableWattHoursCalculation(bool enabled);
    bool isWattHoursCalculationEnabled();
   private:
    unsigned long updated_ms;
    unsigned long started_ms, finished_ms;   
    bool active;
    bool wattHoursCalculationEnabled;

    float voltage;
    float current;
    float power;
    double wattSeconds;
};
#pragma once

#include "CommonTypes.h"
#include "ConfigHelper.h"

#define POWER_SWITCH_PIN D6

enum PsuStatus { PSU_OK, PSU_ERROR, PSU_ALERT };
enum PsuAlert { PSU_ALERT_NONE, PSU_ALERT_VOLTAGE_LOW, PSU_ALERT_LOAD_LOW };

typedef std::function<void()> PsuEventHandler;

class Psu : public PsuInfoProvider {
   public:
    Psu();
    void begin();
    void loop();

    void togglePower();
    void setState(PowerState value, bool forceUpdate = false);
    void setConfig(ConfigHelper*);

    void setOnOn(PsuEventHandler);
    void setOnOff(PsuEventHandler);
    void setOnError(PsuEventHandler);
    void setOnAlert(PsuEventHandler);

    void setOutputVoltage(float voltage);
    float getOutputVoltage();
    String getOutputVoltageInfo();
    String getAlertInfo();
    PsuInfo getInfo();
    String toString();
    PowerState getState();
    String getStateInfo();
    String getDurationInfo();
    float getP();
    float getV();
    float getI();
    double getWh();
    unsigned long getDuration();
    void setWh(double value);
    bool enableWhStore(bool enabled = true);
    bool isWattHoursLogEnabled();

    void printDiag(Print* p);

   private:
    bool storeWh(double value);
    bool restoreWh(double &value);
   private:
    bool storeState(PowerState);
    bool restoreState(PowerState&);

    void onStart();
    void onStop();
    void clearError();
    void setError(PsuStatus err);
    void setAlert(PsuAlert alert);
    void init();
    void storePowerState(PowerState state);
    PowerState restorePowerState();

    PsuStatus status;
    PsuAlert alert;
    unsigned long startedAt, lastUpdated, lastPowerRead, lastCheck;
    bool active;
    bool initialized;
    bool wh_store;
    PsuInfo info;
    ConfigHelper* config;
    PowerState state;
    float outputVoltage;
    PsuEventHandler onPsuOn, onPsuOff, onPsuError, onPsuAlert;
    Print* output = &USE_SERIAL;
    Print* err = &USE_SERIAL;
    static int quadratic_regression(double volt) {
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

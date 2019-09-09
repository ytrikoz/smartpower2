#pragma once

#include "CommonTypes.h"
#include "ConfigHelper.h"

#define POWER_SWITCH_PIN D6

enum PsuStatus { PSU_OK, PSU_ERROR, PSU_ALERT };
enum PsuError { PSU_ERROR_NONE, PSU_ERROR_DC_IN_LOW };
enum PsuAlert { PSU_ALERT_NONE, PSU_ALERT_LOAD_LOW };

typedef std::function<void()> PsuEventHandler;

struct PsuState {
    PsuStatus status;
    PsuError error;
    PsuAlert alert;
    PsuState() { setOK(); }
    void setOK() {
        this->status = PSU_OK;
        this->error = PSU_ERROR_NONE;
        this->alert = PSU_ALERT_NONE;
    }
    bool isOK() {
        update();
        return this->status == PSU_OK;
    }

   private:
    void update() {
        if (error) {
            this->status = PSU_ERROR;
        } else if (alert) {
            this->status = PSU_ALERT;
        } else {
            this->status = PSU_OK;
        }
    }
};

class Psu : public PsuInfoProvider {
   public:
    Psu();
    void togglePower();
    void setConfig(ConfigHelper*);
    void setOnTogglePower(PsuEventHandler);
    void setOnPowerOn(PsuEventHandler);
    void setOnPowerOff(PsuEventHandler);
    void setOnError(PsuEventHandler);
    void setOnAlert(PsuEventHandler);
    void begin();
    void loop();
    PowerState getState();
    String getAlertStr(PsuAlert a);
    String getErrorStr(PsuError e);
    void setOutputVoltage(float voltage);
    float getOutputVoltage();
    PsuInfo getInfo();
    String toString();
    float getP();
    float getV();
    float getI();
    double getWh();
    unsigned long getUptime();
    String getUptimeStr();   
    bool enableWhStore(bool enabled = true);
    bool isWhStoreEnabled();
    void printDiag(Print* p);
    void setWh(double value);
   private:
    void init();
    void setState(PowerState value, bool forceUpdate = false);
    bool storeWh(double value);
    bool restoreWh(double& value);
    bool storeState(PowerState);
    bool restoreState(PowerState&);

    void onStart();
    void onStop();
    void setAlert(PsuAlert);
    void setError(PsuError);
    void clearError();

    bool active;
    bool initialized;
    PsuStatus status;
    PsuAlert alert;
    PsuError error;
    unsigned long startTime, infoUpdated, powerInfoUpdated, lastCheck;
    double outputVoltage;
    bool wh_store;
    PsuInfo info;
    ConfigHelper* config;
    PowerState state;

    Print* out = &USE_SERIAL;
    Print* err = &USE_SERIAL;

    PsuEventHandler onTogglePower, onPowerOn, onPowerOff, onPsuError,
        onPsuAlert;

    static String getStateStr(PowerState state) {
        String str = "";
        if (state == POWER_ON) {
            str = getStrP(str_on);
        } else if (state == POWER_OFF) {
            str = getStrP(str_off);
        }
        return str;
    };

    static int quadratic_regression(double value) {
        double a = 0.0000006562;
        double b = 0.0022084236;
        float c = 4.08;
        double d = b * b - a * (c - value);
        double root = (-b + sqrt(d)) / a;
        if (root < 0)
            root = 0;
        else if (root > 255)
            root = 255;
        return root;
    }
};

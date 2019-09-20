#pragma once

#include "AppModule.h"
#include "CommonTypes.h"
#include "ConfigHelper.h"

#define POWER_SWITCH_PIN D6

enum PsuStatus { PSU_OK, PSU_ERROR, PSU_ALERT };
enum PsuError { PSU_ERROR_NONE, PSU_ERROR_DC_IN_LOW };
enum PsuAlert { PSU_ALERT_NONE, PSU_ALERT_LOAD_LOW };

typedef std::function<void()> PsuEventHandler;

struct PsuState : Printable {
    PsuStatus status;
    PsuError error;
    PsuAlert alert;
    PsuEventHandler onPsuError, onPsuAlert;
    PsuState() { clear(); }
    void clear() {
        status = PSU_OK;
        alert = PSU_ALERT_NONE;
        error = PSU_ERROR_NONE;
    }
    bool isOK() {
        update();
        return this->status == PSU_OK;
    }
    void setError(PsuError e) {
        this->status = PSU_ERROR;
        error = e;
        if (onPsuError) onPsuError();
    }
    void setAlert(PsuAlert a) {
        status = PSU_ALERT;
        alert = a;
        if (onPsuAlert) onPsuAlert();
    }
    size_t printTo(Print& p) const {
        size_t n = 0;
        switch (status) {
            case PSU_ALERT:
                n = p.println(getAlertStr(alert));
                break;
            case PSU_ERROR:
                n = p.println(getErrorStr(error));
                break;
            default:
                break;
        }
        return n;
    }

    static String getAlertStr(PsuAlert alert) {
        String str = "";
        if (alert == PSU_ALERT_NONE) return str;
        str = StrUtils::getStrP(str_alert);
        switch (alert) {
            case PSU_ALERT_LOAD_LOW:
                str += StrUtils::getStrP(str_load_low, false);
                break;
            default:
                str += StrUtils::getStrP(str_unknown);
                break;
        }
        return str;
    }

    static String getErrorStr(PsuError error) {
        String str = "";
        if (error == PSU_ERROR_NONE) return str;
        str = StrUtils::getStrP(str_error);
        switch (error) {
            case PSU_ERROR_DC_IN_LOW:
                str += StrUtils::getStrP(str_low_voltage, false);
                break;
            default:
                str += StrUtils::getStrP(str_unknown);
                break;
        }
        return str;
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

class Psu : public AppModule, public PsuInfoProvider {
   public:
    Psu();
    bool begin();
    void end();
    void loop();
    void printDiag(Print* p);
public: 
    void init();
    void togglePower();
    void setOnTogglePower(PsuEventHandler);
    void setOnPowerOn(PsuEventHandler);
    void setOnPowerOff(PsuEventHandler);
    void setOnError(PsuEventHandler);
    void setOnAlert(PsuEventHandler);
    PowerState getState();
    void setOutputVoltage(float voltage);
    float getOutputVoltage();
    PsuInfo getInfo();
    float getP();
    float getV();
    float getI();
    double getWh();
    unsigned long getUptime();
    bool enableWhStore(bool enabled = true);
    bool isWhStoreEnabled();
    void setWh(double value);
   private:
    void onStart();
    void onStop();
    void setState(PowerState value, bool forceUpdate = false);
    bool storeWh(double value);
    bool restoreWh(double& value);
    bool storeState(PowerState);
    bool restoreState(PowerState&);
    private:
    PowerState state;
    bool active;
    bool initialized;
    PsuState psuState;
    unsigned long startTime, infoUpdated, powerInfoUpdated, lastCheck;
    double outputVoltage;
    bool wh_store;
    PsuInfo info;
    PsuEventHandler onTogglePower, onPowerOn, onPowerOff, onPsuError,
        onPsuAlert;

    static String getStateStr(PowerState state) {
        String str = "";
        if (state == POWER_ON) {
            str = StrUtils::getStrP(str_on);
        } else if (state == POWER_OFF) {
            str = StrUtils::getStrP(str_off);
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

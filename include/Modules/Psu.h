#pragma once

#include "AppModule.h"
#include "CommonTypes.h"
#include "ConfigHelper.h"
#include "PsuLogger.h"

#define POWER_SWITCH_PIN D6

enum PowerState { POWER_ON = 0, POWER_OFF = 1 };
enum PsuStatus { PSU_OK, PSU_ERROR, PSU_ALERT };
enum PsuError { PSU_ERROR_NONE, PSU_ERROR_DC_IN_LOW };
enum PsuAlert { PSU_ALERT_NONE, PSU_ALERT_LOAD_LOW };

typedef std::function<void()> PsuEventHandler;
typedef std::function<void(String &str)> PsuEventMessageHandler;

struct PsuState : Printable {
    PowerState power;
    PsuStatus status;
    PsuError error;
    PsuAlert alert;
    PsuEventMessageHandler errorHandler, alertHandler;
    PsuState() { clear(); }
    void clear() {
        status = PSU_OK;
        alert = PSU_ALERT_NONE;
        error = PSU_ERROR_NONE;
    }

    PowerState getPower(void) { return power; }

    PsuStatus getStatus(void) { return status; }

    String getPowerStateStr() { return getStateStr(power); }

    bool getPower(PowerState state) { return this->power == state; }

    bool getStatus(PsuStatus status) { return this->status == status; }

    bool isOK() {
        update();
        return this->status == status;
    }

    void setOnError(PsuEventMessageHandler h) { errorHandler = h; };

    void setOnAlert(PsuEventMessageHandler h) { alertHandler = h; };

    void setPower(PowerState value) { this->power = value; }

    void setError(PsuError e) {
        this->status = PSU_ERROR;
        error = e;
        if (errorHandler) {
            String errorStr = getErrorStr(e);
            errorHandler(errorStr);
        };
    }

    void setAlert(PsuAlert a) {
        status = PSU_ALERT;
        alert = a;
        if (alertHandler) {
            String alertStr = getAlertStr(a);
            alertHandler(alertStr);
        }
    }

    size_t printTo(Print &p) const {
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

    static String getStateStr(PowerState state) {
        String str = "";
        if (state == POWER_ON) {
            str = StrUtils::getStrP(str_on);
        } else if (state == POWER_OFF) {
            str = StrUtils::getStrP(str_off);
        }
        return str;
    };

    static String getAlertStr(PsuAlert alert) {
        String str = "";
        switch (alert) {
        case PSU_ALERT_NONE:
            break;
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
        if (error == PSU_ERROR_NONE)
            return str;
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

class Psu : public AppModule {
  public:
    PsuLogger *getLogger();
    void setLogger(PsuLogger *);
    void togglePower();
    void powerOff();
    void powerOn();
    void setOnTogglePower(PsuEventHandler);
    void setOnPowerOn(PsuEventHandler);
    void setOnPowerOff(PsuEventHandler);
    void setOnError(PsuEventMessageHandler);
    void setOnAlert(PsuEventMessageHandler);

  public:
    Psu();
    bool begin();
    void end();
    void loop();
    size_t printDiag(Print *p);

  public:
    void init();
    PsuState *getPsuState(void) { return &psuState; }
    void setOutputVoltage(float voltage);
    float getOutputVoltage();
    String getPowerStateStr();
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
    void onPowerOff();
    void onPowerOn();
    void setState(PowerState value, bool force = false);
    bool storeWh(double value);
    bool restoreWh(double &value);
    bool storeState(PowerState);
    bool restoreState(PowerState &);

  private:
    PsuLogger *logger;
    PsuState psuState;
    bool active;
    bool initialized;
    unsigned long startTime, infoUpdated, powerInfoUpdated, loggerUpdated,
        lastCheck;
    double outputVoltage;
    bool wh_store;
    PsuInfo info;
    PsuEventHandler togglePowerHandler, powerOnHandler, powerOffHandler;

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

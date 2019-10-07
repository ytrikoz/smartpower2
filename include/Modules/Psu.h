#pragma once

#include "AppModule.h"
#include "CommonTypes.h"
#include "ConfigHelper.h"
#include "PsuLogger.h"

#define POWER_SWITCH_PIN D6

enum PsuState { POWER_ON = 0, POWER_OFF = 1 };

enum PsuStatus { PSU_OK, PSU_ERROR, PSU_ALERT };

enum PsuError { PSU_ERROR_NONE, PSU_ERROR_LOW_VOLTAGE };

enum PsuAlert { PSU_ALERT_NONE, PSU_ALERT_LOAD_LOW };

typedef std::function<void()> PsuEventHandler;

typedef std::function<void(PsuStatus, String &)> PsuStatusHandler;

typedef std::function<void(PsuState)> PsuStateHandler;

class Psu : public AppModule {
  public:
    Psu();
    void setConfig(Config *config);
    bool begin();
    void end();
    void loop();
    size_t printDiag(Print *p);

  public:
    void togglePower();
    void powerOff();
    void powerOn();
    void setLogger(PsuLogger *);
    PsuLogger *getLogger();
    void setOnStateChange(PsuStateHandler);
    void setOnStatusChange(PsuStatusHandler);

    void setState(PsuState);
    PsuState getState(void);
    bool checkState(PsuState);
    String getStateStr();
    PsuStatus getStatus(void);
    bool checkStatus(PsuStatus);
    String getMessage();

  public:
    void init();

    void setVoltage(float voltage);
    float getVoltage();
    String getPsuStateStr();
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
    void setError(PsuError value);
    void setAlert(PsuAlert value);
    void setState(PsuState value, bool force = false);
    bool storeWh(double value);
    bool restoreWh(double &value);
    bool storeState(PsuState);
    bool restoreState(PsuState &);
    void reset();
    void updateStatus();
    void updateVoltage();
    PGM_P getAlertStrP(PsuAlert);
    PGM_P getErrorStrP(PsuError value);

  private:
    PsuState state;
    PsuStatus status;
    PsuError error;
    PsuAlert alert;
    PsuLogger *logger;
    PsuStatusHandler statusHandler;
    PsuStateHandler stateHandler;

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

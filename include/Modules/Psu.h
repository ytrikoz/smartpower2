#pragma once

#include "AppModule.h"
#include "CommonTypes.h"
#include "ConfigHelper.h"
#include "PsuLogger.h"

#define POWER_SWITCH_PIN D6

typedef std::function<void(PsuState, PsuStatus)> PsuStateChangeHandler;

typedef std::function<void(PsuInfo)> PsuInfoHandler;

class Psu : public AppModule {
  public:
    Psu();
    void setConfig(Config *config);
    bool begin();
    void loop();
    size_t printDiag(Print *p);

  public:
    void setLogger(PsuLogger *);
    PsuLogger *getLogger();
    void togglePower();
    void powerOff();
    void powerOn();
    void setOnStateChange(PsuStateChangeHandler);
    void setOnPsuInfo(PsuInfoHandler);
    PsuState getState(void);
    PsuStatus getStatus(void);
    PsuAlert getAlert(void);
    PsuError getError(void);
    bool checkState(PsuState);
    bool checkStatus(PsuStatus);

  public:
    void setVoltage(float voltage);
    float getVoltage();
    PsuInfo getInfo();
    float getP();
    float getV();
    float getI();
    double getWh();
    unsigned long getUptime();
    bool enableWhStore(bool enabled = true);
    bool isWhStoreEnabled(void);
    void setWh(double value);

  private:
    void setOk(void);
    void setStatus(PsuStatus value);
    void setError(PsuError value);
    void setAlert(PsuAlert value);
    void setState(PsuState value);
    bool storeWh(double value);
    bool restoreWh(double &value);
    bool storeState(PsuState);
    bool restoreState(PsuState &);

  private:
    PsuState state;
    PsuStatus status;
    PsuError error;
    PsuAlert alert;
    PsuLogger *logger;
    PsuStateChangeHandler stateChangeHandler;
    PsuInfoHandler psuInfoHandler;

    unsigned long startTime, infoUpdated, powerInfoUpdated, loggerUpdated,
        lastCheck;
    double outputVoltage;
    bool wh_store;
    PsuInfo info;

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

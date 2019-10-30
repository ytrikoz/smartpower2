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
    bool checkVoltageRange(float value);
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
    void clearErrorsAndAlerts(void);
    void setStatus(PsuStatus value);
    void setError(PsuError value);
    void setAlert(PsuAlert value);
    void setState(PsuState value);
    bool storeWh(double value);
    bool restoreWh(double &value);
    bool storeState(PsuState);
    bool restoreState(PsuState &);

  private:
    int quadratic_regression(double value);

    PsuState state;
    PsuStatus status;
    PsuError error;
    PsuAlert alert;
    PsuLogger *logger;
    PsuStateChangeHandler stateChangeHandler;
    PsuInfoHandler psuInfoHandler;

    unsigned long startTime, infoUpdated, powerInfoUpdated, loggerUpdated,
        lastCheck, lastStore;
    double outputVoltage;
    bool wh_store;
    PsuInfo info;
    bool alterRange;
};

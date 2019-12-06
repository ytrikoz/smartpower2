#pragma once

#include "AppModule.h"
#include "CommonTypes.h"
#include "ConfigHelper.h"
#include "PsuLogger.h"
#include "ina231.h"
#include "mcp4652.h"

#define POWER_SWITCH_PIN D6

typedef std::function<void(PsuState, PsuStatus)> PsuStateChangeHandler;
typedef std::function<void(PsuInfo)> PsuInfoHandler;

class Psu : public AppModule {
   public:
    Psu() : AppModule(MOD_PSU) {
        ina231_configure();
        mcp4652_init();
    };
    size_t onDiag(Print *p) override;

   protected:
    bool onInit() override;
    bool onStart() override;
    void onLoop() override;

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
    const PsuInfo getInfo();
    void setVoltage(const double voltage);
    bool checkVoltageRange(const double value);
    const float getVoltage();
    const float getP();
    const float getV();
    const float getI();
    const double getWh();
    const unsigned long getUptime();
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

#pragma once

#include "Module.h"
#include "PsuUtils.h"

#define POWER_SWITCH_PIN D6

class PsuModule : public Module {
   public:
    PsuModule(PsuListener* listener) : Module(), listener_(listener) {};

    size_t onDiag(Print *p) override;

   protected:
    bool onInit() override;
    bool onStart() override;
    void onLoop() override;

   public:
    void togglePower();
    void powerOff();
    void powerOn();
    void setOnStateChange(PsuStateChangeHandler);
    void setOnData(PsuDataHandler);
    PsuState getState(void);
    PsuStatus getStatus(void);
    PsuAlert getAlert(void);
    PsuError getError(void);
    bool checkState(PsuState);
    bool checkStatus(PsuStatus);

   public:
    const PsuData getInfo();
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
    PsuListener* listener_;
    PsuDataHandler dataHandler_;

    PsuState state;
    PsuStatus status;
    PsuError error;
    PsuAlert alert;

    PsuStateChangeHandler stateChangeHandler;

    unsigned long startTime, infoUpdated, powerInfoUpdated, listenerUpdate_,
        lastCheck, lastStore;
    double outputVoltage;
    bool wh_store;
    PsuData info;
    bool alterRange;
};

#pragma once

#include "Core/Module.h"

#include "Utils/PsuUtils.h"

#define POWER_SWITCH_PIN D6

namespace Modules {

class Psu : public Module {
   public:
    Psu(PsuListener* listener) : Module(), listener_(listener), startTime_(0), infoUpdated_(0), powerInfoUpdated_(0), listenerUpdate_(0), lastStore_(0){}

    void onDiag(const JsonObject&) override;

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
    const unsigned long getPsuUptime();
    bool enableWhStore(bool enabled = true);
    bool isWhStoreEnabled(void);
    void setWh(double value);

   private:
    void clearErrorsAndAlerts(void);
    void setStatus(PsuStatus value);
    void setError(PsuError value);
    void setAlert(PsuAlert value);
    void applyState(PsuState value);
    bool storeWh(double value);
    bool restoreWh(double& value);
    bool storeState(PsuState);
    bool restoreState(PsuState&);

   private:
    PsuListener* listener_;
    PsuDataHandler dataHandler_;
    PsuStateChangeHandler stateChangeHandler_;

    PsuState state_;
    PsuStatus status_;
    PsuError error_;
    PsuAlert alert_;
    double outputVoltage_;
    bool wh_store_;
    PsuData info_;
    bool alterRange_;

    unsigned long startTime_, infoUpdated_, powerInfoUpdated_, listenerUpdate_,
        lastCheck_, lastStore_;
};

}  // namespace Modules
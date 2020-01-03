#pragma once

#include "Core/Module.h"

#define POWER_SWITCH_PIN D6

namespace Modules {

typedef std::function<void(const PsuState, const String)> PsuStateChangeHandler;
typedef std::function<void(const PsuStatus, const String)> PsuStatusChangeHandler;

class Psu : public Module {
   public:
    Psu() : Module(), startTime_(0), infoUpdated_(0), powerInfoUpdated_(0), listenerUpdate_(0), lastStore_(0), lastStoredWh_(0), lastStoredState_(-1) {}

    void onDiag(const JsonObject&) override;

    void togglePower();
    void powerOff();
    void powerOn();

    void setOnStateChange(PsuStateChangeHandler);
    void setOnStatusChange(PsuStatusChangeHandler);
    void setOnData(PsuDataListener*);

    void setWh(double value);
    void setOutputVoltage(const float value);

    BootPowerState getBootPowerState() const;
    bool isWhStoreEnabled() const;
    bool isStateStoreEnabled() const;
    bool isPowerOn() const;

    PsuState getState(void) const;
    String getStateStr(void) const;
    String getStateStr(const PsuState) const;
    PsuStatus getStatus(void) const;
    String getStatusStr(void) const;

    PsuAlert getAlert(void) const;
    PsuError getError(void) const;
    float getOutputVoltage() const;
    const unsigned long getUptime() const;
    const PsuData getData() const;
    String getErrorStr() const;
    String getAlertStr() const;

   protected:
    bool onInit() override;
    bool onStart() override;
    void onLoop() override;
    bool onConfigChange(const ConfigItem, const String&) override;

   private:
    void onStateChangeEvent(PsuState);
    void applyVoltage(const float);

    void clearErrorsAndAlerts(void);
    void setStatus(PsuStatus);
    void setError(PsuError);
    String getErrorStr(const PsuError) const;
    void setAlert(PsuAlert);
    String getAlertStr(const PsuAlert) const;

    bool storeWh(double);
    bool restoreWh(double&);
    bool storeState(PsuState);
    bool restoreState(PsuState&);

    uint8_t mapVoltage(const float);
    bool mapState(const PsuState);
    uint8_t quadratic_regression(const float value, const float c) const;

   private:
    PsuDataListener* dataListener_;
    PsuStateChangeHandler stateChangeHandler_;
    PsuStatusChangeHandler statusChangeHandler_;

    PsuState state_;
    PsuStatus status_;
    PsuError error_;
    PsuAlert alert_;
    PsuData info_;
    unsigned long startTime_, infoUpdated_, powerInfoUpdated_, listenerUpdate_,
        lastStore_, lastCheck_;
    double lastStoredWh_;
    int lastStoredState_;
};

}  // namespace Modules
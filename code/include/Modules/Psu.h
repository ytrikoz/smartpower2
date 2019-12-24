#pragma once

#include "Core/Module.h"

#include "Utils/PsuUtils.h"

#define POWER_SWITCH_PIN D6

namespace Modules {

class Psu : public Module {
   public:
    Psu() : Module(), startTime_(0), infoUpdated_(0), powerInfoUpdated_(0), listenerUpdate_(0), lastStore_(0), lastStoredWh_(0), lastStoredState_(-1){}

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
    bool isWhStoreEnabled() const ;
    bool isStateStoreEnabled() const ;
    bool isPowerOn() const;
    
    PsuState getState(void) const;
    PsuStatus getStatus(void) const;
    PsuAlert getAlert(void) const ;
    PsuError getError(void) const ;
    float getOutputVoltage() const ;    
    const unsigned long getUptime() const;
    const PsuData getInfo() const;
   protected:
    bool onInit() override;
    bool onStart() override;
    void onLoop() override;
    bool onConfigChange(const ConfigItem param, const String& value) override;
    Error onExecute(const String &param, const String &value) override;   
   private:          
    void onStateChangeEvent(PsuState value);
    void applyVoltage(const float value);

    void clearErrorsAndAlerts(void);
    void setStatus(PsuStatus value);
    void setError(PsuError value);
    void setAlert(PsuAlert value);

    bool storeWh(double value);
    bool restoreWh(double& value);
    bool storeState(PsuState);
    bool restoreState(PsuState&);

    uint8_t mapVoltage(const float value);
    bool mapState(const PsuState state);
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
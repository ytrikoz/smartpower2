#pragma once

#include "Core/Module.h"

#include "Utils/PsuUtils.h"

#define POWER_SWITCH_PIN D6

namespace Modules {

class Psu : public Module {
   public:
    Psu() : Module(), startTime_(0), infoUpdated_(0), powerInfoUpdated_(0), listenerUpdate_(0), lastStore_(0){}

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
   
   private:          
    void applyState(PsuState value);
    void applyVoltage(const float value);

    void clearErrorsAndAlerts(void);
    void setStatus(PsuStatus value);
    void setError(PsuError value);
    void setAlert(PsuAlert value);

    bool storeWh(double value);
    bool restoreWh(double& value);
    bool storeState(PsuState);
    bool restoreState(PsuState&);

    int mapVoltage(const float value);
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
        lastCheck_, lastStore_;
};

}  // namespace Modules
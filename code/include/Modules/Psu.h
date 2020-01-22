#pragma once

#include "Core/Module.h"

#define POWER_SWITCH_PIN D6

struct PsuData : Printable {
    unsigned long time;
    float V;
    float I;
    float P;
    double Wh;

   public:
    PsuData() { time = V = I = P = Wh = 0; }

    PsuData(unsigned long time_ms, float V, float I, float P, double Wh)
        : time(time_ms), V(V), I(I), P(P), Wh(Wh){};

    void reset(void) { time = V = I = P = Wh = 0; }

    char* prettyNumber(char* buf, float value, const char* name) const {
        char tmp[8];
        if (I < 0.5) {
            strcpy(buf, dtostrf(value * 1000, 3, 0, tmp));
            strcat(buf, "m");
        } else {
            strcpy(buf, dtostrf(value, 2, 2, tmp));
        }
        strcat(buf, name);
        return buf;
    }

    size_t toPretty(char* buf) const {
        char tmp[32];
        memset(tmp, 0, 32);
        // Volt
        strcat(buf, dtostrf(V, 2, 2, tmp));
        strcat(buf, "V ");
        // Amper
        strcat(buf, prettyNumber(tmp, I, "A "));
        // Watt
        strcat(buf, dtostrf(P, 2, 2, tmp));
        strcat(buf, "W");

        return strlen(buf);
    }

    String toJson() const {
        String res = "{";
        res += "\"v\":" + String(V, 3) + ",";
        res += "\"i\":" + String(I, 3) + ",";
        res += "\"p\":" + String(P, 3) + ",";
        res += "\"wh\":" + String(Wh, 6) + "}";
        return res;
    }

    size_t printTo(Print& p) const {
        size_t n = 0;
        n += p.print(V, 3);
        n += p.print("V, ");
        n += p.print(I, 3);
        n += p.print("A, ");
        n += p.print(P, 3);
        n += p.print("W, ");
        n += p.print(Wh, 3);
        n += p.print("Wh");
        return n;
    }
};

class PsuDataListener {
   public:
    virtual void onPsuData(PsuData& item){};
};

namespace Modules {

typedef std::function<void(const PsuState, const String)> PsuStateChangeHandler;

class Psu : public Module {
   public:
    Psu() : Module(), startTime_(0), infoUpdated_(0), powerInfoUpdated_(0), listenerUpdate_(0), lastStore_(0), lastStoredWh_(0), lastStoredState_(-1) {}

    void onDiag(const JsonObject&) override;

    void togglePower();
    void powerOff();
    void powerOn();

    void setOnStateChange(PsuStateChangeHandler);
    void setOnError(ErrorHandler);
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
    
    float getOutputVoltage() const;
    const unsigned long getUptime() const;
    const PsuData* getData() const;

   protected:
    bool onInit() override;
    bool onStart() override;
    void onLoop() override;
    bool onConfigChange(const ConfigItem, const String&) override;

   private:
    void onStateChangeEvent(PsuState);
    void applyVoltage(const float);

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
    PsuState state_;   
    ErrorHandler errorHandler_;
    PsuData metering_;
    unsigned long startTime_, infoUpdated_, powerInfoUpdated_, listenerUpdate_,
        lastStore_, lastCheck_;
    double lastStoredWh_;
    int lastStoredState_;
};

}  // namespace Modules
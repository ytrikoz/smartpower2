#pragma once

#include "Actions.h"
#include "ina231.h"

namespace Actions {

class PowerSetAvg : public NumericAction {
   public:
    void setParam(long number);
    void exec(Print* p);
    size_t printTo(Print& p) const;
   private:
    long param;
};

inline void PowerSetAvg::setParam(long number) { param = number; }
inline void PowerSetAvg::exec(Print* p) {
#ifdef DEBUG_SHELL 
	DEBUG.print("[shell] power avg ");
	DEBUG.print(param);
#endif
	ina231_set_avg((uint8_t)param);}

inline size_t PowerSetAvg::printTo(Print& p) const {
    size_t res = p.println(F("show clients"));
    return res;
}

PowerSetAvg* powerSetAvg = new PowerSetAvg();

}  // namespace Actions
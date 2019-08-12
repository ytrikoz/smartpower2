#pragma once

#include "Actions.h"

#include "ina231.h"

namespace Actions {

class PowerAvg : public NumericAction {
   public:
    PowerAvg(const char* str, size_t param);
    void exec(Print* p);
};

PowerAvg::PowerAvg(const char* str, size_t param) : NumericAction(str, param) {}

void PowerAvg::exec(Print* p) { 
    ina231_set_avg((uint8_t)param); 
}

}  // namespace Actions
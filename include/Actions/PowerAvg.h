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
    INA231_AVERAGES avg;

    if (param >= 1024)
        avg = AVG_1024;
    else if (param >= 512)
        avg = AVG_512;
    else if (param >= 256)
        avg = AVG_256;
    else if (param >= 128)
        avg = AVG_128;
    else if (param >= 64)
        avg = AVG_64;
    else if (param >= 16)
        avg = AVG_16;
    else if (param >= 4)
        avg = AVG_4;
    else
        avg = AVG_1;

    DEBUG.printf("PowerAvg::exec(%d)", avg);
    DEBUG.println();

    ina231_set_avg(avg);
}

}  // namespace Actions
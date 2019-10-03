#pragma once

#include "Actions.h"

#include "ina231.h"

namespace Actions {

class PowerAvg : public NumericAction {
  public:
    PowerAvg(size_t param);
    void exec(Print *p);
    void print_config_reg(Print *p);
};

PowerAvg::PowerAvg(size_t param) : NumericAction(param) {}

void PowerAvg::print_config_reg(Print *p) {
    uint16_t config = ina231_read_config();
    uint8_t byte[2];
    byte[0] = (uint8_t)((config >> 8) & 0x00ff);
    byte[1] = (uint8_t)(config & 0x00ff);
    p->print(byte[0], BIN);
    p->print(' ');
    p->println(byte[1], BIN);
}

void PowerAvg::exec(Print *p) {
    INA231_AVERAGES avg;
    size_t nearest;
    if (param >= 1024) {
        avg = AVG_1024;
        nearest = 1024;
    } else if (param >= 512) {
        avg = AVG_512;
        nearest = 512;
    } else if (param >= 256) {
        avg = AVG_256;
        nearest = 256;
    } else if (param >= 128) {
        avg = AVG_128;
        nearest = 128;
    } else if (param >= 64) {
        avg = AVG_64;
        nearest = 64;
    } else if (param >= 16) {
        avg = AVG_16;
        nearest = 16;
    } else if (param >= 4) {
        avg = AVG_4;
        nearest = 4;
    } else {
        avg = AVG_1;
        nearest = 1;
    }
    p->print(StrUtils::getStrP(str_avg));
    p->print(StrUtils::getStrP(str_set));
    p->println(nearest);

    ina231_set_avg(avg);

    print_config_reg(p);
}

} // namespace Actions
#pragma once

#include "Hardware/ina231.h"
#include "Utils/PrintUtils.h"

namespace Actions {

class Avg {
   public:
    static void print_config(Print *p) {
        uint16_t config = ina231_read_config();
        uint8_t byte[2];
        byte[0] = (uint8_t)((config >> 8) & 0x00ff);
        byte[1] = (uint8_t)(config & 0x00ff);
        p->print(byte[0], BIN);
        p->print(' ');
        p->println(byte[1], BIN);
    }

    static void set(Print *p, int param) {
        INA231_AVERAGES avg;
        size_t value;
        if (param >= AVG_1024) {
            avg = AVG_1024;
            value = 1024;
        } else if (param >= AVG_512) {
            avg = AVG_512;
            value = 512;
        } else if (param >= AVG_256) {
            avg = AVG_256;
            value = 256;
        } else if (param >= AVG_128) {
            avg = AVG_128;
            value = 128;
        } else if (param >= AVG_64) {
            avg = AVG_64;
            value = 64;
        } else if (param >= AVG_16) {
            avg = AVG_16;
            value = 16;
        } else if (param >= AVG_4) {
            avg = AVG_4;
            value = 4;
        } else {
            avg = AVG_1;
            value = 1;
        }
        ina231_set_avg(avg);       
        PrintUtils::print(p, FPSTR(str_avg), value);
        PrintUtils::println(p);
    }
};

}  // namespace Actions
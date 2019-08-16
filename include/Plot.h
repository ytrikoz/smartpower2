#pragma once

#include "Arduino.h"
#include "Consts.h"

#define PLOT_COLS 8
#define PLOT_ROWS 2

struct PlotData {
    float cols[PLOT_COLS] = {0};
    size_t size = 0;
    float min_value = 16384;
    float max_value = -16364;
};


float map_to_plot_min_max(PlotData *pd, uint8_t x);

size_t fill_data(PlotData *data, float *vals, size_t size);
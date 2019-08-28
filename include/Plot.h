#pragma once

#include "Arduino.h"
#include "Consts.h"

#define PLOT_COLS 8
#define PLOT_ROWS 2

#define UNSET_MAX -16364
#define UNSET_MIN 16364

struct PlotData {
    float cols[PLOT_COLS];
    size_t size = 0;
    float min_value = UNSET_MIN;
    float max_value = UNSET_MAX;
    float avg_value = 0; 
};

void value_max_min(float value, float& max, float& min);

float map_to_plot_min_max(PlotData *pd, uint8_t x);

size_t fill_data(PlotData *data, float *vals, size_t size);

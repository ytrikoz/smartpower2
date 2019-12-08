#pragma once

#include "Arduino.h"
#include "Consts.h"

#define PLOT_COLS 8
#define PLOT_ROWS 2

#define UNSET_MAX -16364
#define UNSET_MIN 16364

struct PlotData {
    float columns[PLOT_COLS];
    size_t size = 0;
    float min_value = UNSET_MIN;
    float max_value = UNSET_MAX;
    float avg_value = 0;
};

void value_max_min(float value, float &max, float &min);

float compress(PlotData *data, size_t column);

size_t group(PlotData *data, float *vals, size_t size);

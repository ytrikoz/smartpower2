#include "Plot.h"

float map_to_plot_min_max(PlotData *pd, uint8_t x) {
    float val = pd->cols[x];
    float min = pd->min_value;
    float max = pd->max_value - min;
    return 1 + floor((val - min) / max * (PLOT_ROWS * 8 - 2));
}

size_t fill_data(PlotData *data, float *vals, size_t size) {
    size_t group_size, offset, col_count;
    if (size <= PLOT_COLS) {
        col_count = size;
        group_size = 1;
        offset = 0;
    } else {
        col_count = PLOT_COLS;
        group_size = floor((float)size / PLOT_COLS);
        offset = size - group_size * PLOT_COLS;
    }

    data->min_value = 16384;
    data->max_value = -16384;
    data->size = col_count;

#ifdef DEBUG_PLOT
    DEBUG.printf("size %d offset %d by %d", size, offset, group_size);
    DEBUG.println();
#endif

    for (uint8_t col_n = 0; col_n < col_count; ++col_n) {
        float group_sum = 0;
        for (uint8_t value_n = 0; value_n < group_size; ++value_n)
            group_sum += vals[offset + (col_n * group_size) + value_n];
        float group_avg = group_sum / group_size;
#ifdef DEBUG_PLOT
        DEBUG.printf("#%d sum %.4f avg %.4f", col_n, group_sum, group_avg);
        DEBUG.println();
#endif
        if (data->min_value > group_avg) data->min_value = group_avg;
        if (data->max_value < group_avg) data->max_value = group_avg;

        data->cols[col_n] = group_avg;
    }
#ifdef DEBUG_PLOT
    DEBUG.printf("cols %d min %2.4f max %2.4f", col_count, data->min_value,
                 data->max_value);
    DEBUG.println();
#endif

    return col_count;
}

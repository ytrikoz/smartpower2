#include "Plot.h"

float compress(PlotData *data, size_t column) {
    float value = data->columns[column];
    float min = data->min_value;
    float limit_range = PLOT_ROWS * 8 - 2;
    float full_range = data->max_value - data->min_value;
    return 1 + ((value - min) / full_range * limit_range);
}

size_t group(PlotData *data, float *array, size_t array_size) {
    size_t group_size, offset, column_count;
    if (array_size <= PLOT_COLS) {
        column_count = array_size;
        group_size = 1;
        offset = 0;
    } else {
        column_count = PLOT_COLS;
        group_size = floor((float)array_size / PLOT_COLS);
        offset = array_size % (group_size * PLOT_COLS);
    }

    data->min_value = 16384;
    data->max_value = -16384;
    data->size = column_count;

    for (size_t column_pos = 0; column_pos < column_count; ++column_pos) {
        float group_sum = 0;
        for (size_t array_pos = 0; array_pos < group_size; ++array_pos)
            group_sum += array[offset + (column_pos * group_size) + array_pos];
        float group_avg = group_sum / group_size;

        if (data->min_value > group_avg)
            data->min_value = group_avg;
        if (data->max_value < group_avg)
            data->max_value = group_avg;

        data->columns[column_pos] = group_avg;
    }

    return column_count;
}

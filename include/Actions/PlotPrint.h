#pragma once

#include "Actions.h"

namespace Actions {

class PlotPrint : public Action {
   public:
    void exec(Print* p);
};

void PlotPrint::exec(Print* p) {
    PlotData* data = app.getDisplay()->getData();
    for (uint8_t x = 0; x < data->size; ++x) {
        uint8_t y = map_to_plot_min_max(data, x);
        char tmp[PLOT_ROWS * 8 + 1];
        StrUtils::strfill(tmp, '*', y);
        p->printf("#%d %2.4f ", x + 1, data->cols[x]);
        p->print(tmp);
        p->println();
    }
}

}  // namespace Actions
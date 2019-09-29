#pragma once

#include "Actions.h"

namespace Actions {

class ShowNtp : public Action {
   public:
    void exec(Print* p);
};

void ShowNtp::exec(Print* p) {
    auto mod = app.getInstance(MOD_NTP);
    if (mod) {
        mod->printDiag(p);
    } else {
        p->println(StrUtils::getStrP(str_disabled));
    }
}

}  // namespace Actions
#pragma once

#include "Actions.h"

namespace Actions {

class ShowWifi : public Action {
   public:
    void exec(Print* p);
};

void ShowWifi::exec(Print* p) {
    Wireless::printDiag(p);
}

}  // namespace Actions

#pragma once

#include "Actions.h"

namespace Actions {

class ShowDiag : public Action {
   public:
    void exec(Print* p);
};

void ShowDiag::exec(Print* p) {  }

}  // namespace Actions

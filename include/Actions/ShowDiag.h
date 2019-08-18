#pragma once

#include "Actions.h"

namespace Actions {

class ShowDiag : public EmptyParamAction {
   public:
    void exec(Print* p);
};

void ShowDiag::exec(Print* p) { Wireless::printDiag(p); }

}  // namespace Actions

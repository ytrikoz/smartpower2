#pragma once

#include "ParameterlessAction.h"

namespace Actions {

class ShowDiag : public ParameterlessAction {
   public:
    void Execute(Print* p);
    size_t printTo(Print& p) const;
};

inline void ShowDiag::Execute(Print* p) { Wireless::printDiag(p); }

inline size_t ShowDiag::printTo(Print& p) const {
    size_t res = p.println("show diag");
    return res;
}

ShowDiag* showDiag = new ShowDiag();

}  // namespace Actions

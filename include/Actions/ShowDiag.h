#pragma once

#include "Actions.h"

namespace Actions {

class ShowDiag : public EmptyParamAction {
   public:
    void exec(Print* p);
    size_t printTo(Print& p) const;
};

inline void ShowDiag::exec(Print* p) { Wireless::printDiag(p); }

inline size_t ShowDiag::printTo(Print& p) const {
    size_t res = p.println("show diag");
    return res;
}

ShowDiag* showDiag = new ShowDiag();

}  // namespace Actions

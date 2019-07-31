#pragma once

#include "ParameterlessCommand.h"
#include "wireless.h"

namespace shell {

class ShowDiag : public ParameterlessCommand {
   public:
    void Execute(Print* p);
    size_t printTo(Print& p) const;
};

inline void ShowDiag::Execute(Print* p) { wireless::printDiag(p); }

inline size_t ShowDiag::printTo(Print& p) const {
    size_t res = p.println("show diag");
    return res;
}

ShowDiag *showDiag = new ShowDiag();

}  // namespace shell

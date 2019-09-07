#pragma once

#include "Actions.h"

namespace Actions {

class LogPrint : public NumericAction {
   public:
    LogPrint(size_t param);
    void exec(Print* p);
};

LogPrint::LogPrint(size_t param) : NumericAction(param) {}

void LogPrint::exec(Print* p) { psuLog->print(p, (PsuLogItem)param); }

}  // namespace Actions
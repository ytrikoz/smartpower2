#pragma once

#include "Actions.h"

namespace Actions
{

class ShowPsu : public Action
{
public:
    void exec(Print *p) { app.printDiag(MOD_PSU, p); };
};

} // namespace Actions

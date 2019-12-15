#pragma once

#include "Print.h"

namespace Cli {

extern Print* setOutput(Print*);

class Runner {
   public:
    virtual void run(const char*, Print*);
};

}  // namespace Cli

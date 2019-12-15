#pragma once

#include <Arduino.h>

#include <SimpleCLI.h>

namespace Cli {

void init();
SimpleCLI* get();
Print* setOutput(Print*);
Print* getOutput();

}  // namespace Cli

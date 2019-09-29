#pragma once

#include <Print.h>
#include <SimpleCLI.h>

namespace Cli {

void init();
void setOutput(Print *);
void close();
bool active();

}  // namespace Cli

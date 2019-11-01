#pragma once

#include <Print.h>
#include <SimpleCLI.h>

namespace Cli {

void setOutput(Print *);
void init();
void close();
bool active();

} // namespace Cli

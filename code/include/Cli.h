#pragma once

#include <Arduino.h> 

namespace Cli {

void setOutput(Print *);
void init();
void close();
bool active();

}  // namespace Cli

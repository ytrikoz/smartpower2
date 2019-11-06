#pragma once

#include "Arduino.h"

#include "mcurses.h"

void editLine(char *str, uint8_t lineLength);

int16_t editInt16(int16_t initialNumber);
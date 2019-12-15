#pragma once

#include "CommonTypes.h"

int quadratic_regression(const double value, const bool alterRange = false);

String getStatusStr(const PsuStatus status);

String getStateStr(const PsuState state);

String getErrorStr(const PsuError value);

String getAlertStr(const PsuAlert value);
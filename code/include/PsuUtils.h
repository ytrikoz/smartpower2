#pragma once

#include "CommonTypes.h"

int quadratic_regression(double value, bool alterRange = false);

String getStatusStr(PsuStatus status);

String getStateStr(PsuState state);

String getErrorStr(PsuError value);

String getAlertStr(PsuAlert value);
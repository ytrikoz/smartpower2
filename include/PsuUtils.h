#pragma once

#include "CommonTypes.h"

String getStatusStr(PsuStatus status);

String getStateStr(PsuState state);

PGM_P getStateStrP(PsuState state);

PGM_P getStatusStrP(PsuStatus status);

PGM_P getErrorStrP(PsuError value);

PGM_P getAlertStrP(PsuAlert value);
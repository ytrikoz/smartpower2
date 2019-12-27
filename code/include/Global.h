#pragma once

#include "App.h"
#include "MainLog.h"
#include "BootWatcher.h"
#include "Powerlog.h"
#include "LoopTiming.h"
#include "Wireless.h"

extern App app;
extern BootWatcher boot;
extern MainLog mainlog;

extern Wireless* wireless;
extern ConfigHelper* config;
extern PowerLog* powerlog;
extern LoopTimer* loopTimer;


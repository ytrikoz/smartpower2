#pragma once

#include "App.h"
#include "MainLog.h"
#include "BootWatcher.h"
#include "PowerLog.h"
#include "LoopTiming.h"
#include "Wireless.h"

extern App app;
extern MainLog mainlog;

extern BootWatcher* boot;
extern Wireless* wireless;
extern ConfigHelper* config;
extern PowerLog* powerlog;
extern LoopTimer* loopTimer;


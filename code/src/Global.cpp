#include "Global.h"

App app;
BootWatcher boot;
Logger syslog;

ConfigHelper* config;
PowerLog* powerlog;
LoopTimer* loopTimer = nullptr;
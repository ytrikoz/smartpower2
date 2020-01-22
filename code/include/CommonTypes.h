#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <time.h>
#include <IPAddress.h>

#include "Consts.h"
#include "Strings.h"
#include "Core/Error.h"

#include "Utils/TimeUtils.h"

enum WebPageEnum {
    PAGE_HOME = 1,
    PAGE_OPTIONS = 2,
    PAGE_INFO = 3
};

enum BootPowerState {
    BOOT_POWER_OFF = 0,
    BOOT_POWER_ON = 1,
    BOOT_POWER_LAST_STATE = 2
};

enum NetworkMode {
    NETWORK_OFF,
    NETWORK_STA,
    NETWORK_AP,
    NETWORK_AP_STA
};

enum NetworkStatus { NETWORK_DOWN = 0,
                     NETWORK_UP = 1 };

enum LogLevel { LEVEL_ERROR,
                LEVEL_WARN,
                LEVEL_INFO };

enum PsuState { POWER_OFF,
                POWER_ON };

enum StoreState { SS_UNSET,
                  SS_CLOSED,
                  SS_READING,
                  SS_WRITING,
                  SS_EOF };

enum StoreError {
    SE_NONE = 0,
    SE_INVALID,
    SE_NOT_EXIST,
    SE_ERROR_CLOSE,
    SE_ERROR_READ,
    SE_ERROR_WRITE,
};

struct NetInfo {
    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;
    NetInfo(){};
    NetInfo(ip_info& info) {
        ip = IPAddress(info.ip.addr);
        subnet = IPAddress(info.netmask.addr);
        gateway = IPAddress(info.gw.addr);
    }
};


enum EOLType { CRLF,
               LFCR,
               LF,
               CR };

enum MoveDirection { MD_LEFT,
                     MD_RIGHT,
                     MD_UP,
                     MD_DOWN };


enum ModuleEnum {
    MOD_LED,
    MOD_BTN,
    MOD_CLOCK,
    MOD_PSU,
    MOD_DISPLAY,
    MOD_CONSOLE,
    MOD_NETSVC,
    MOD_TELNET,
    MOD_UPDATE,
    MOD_SYSLOG,
    MOD_WEB
};

enum ModuleState {
    STATE_INIT,
    STATE_INIT_FAILED,
    STATE_INIT_COMPLETE,
    STATE_START_FAILED,
    STATE_ACTIVE
};

typedef std::function<void(const Error)> ErrorHandler;

#define MODULES_COUNT 11
#pragma once

#include <SimpleCLI.h>

#include "Strings.h"

namespace Cli {

enum CommandAction {
    ACTION_UNKNOWN,
    ACTION_PRINT,
    ACTION_RESET,
    ACTION_SAVE,
    ACTION_LOAD,
    ACTION_APPLY,
    ACTION_STATUS,
    ACTION_LOG,
    ACTION_AVG,
    ACTION_ON,
    ACTION_OFF,
    ACTION_RESTART,
    ACTION_BACKLIGHT,
    ACTION_UPTIME,
    ACTION_TIME,
    ACTION_WOL,
    ACTION_LIST,
    ACTION_CLEAR,
    ACTION_TEST,
    ACTION_SET,
    ACTION_CONTROL,
    ACTION_CONFIG,
    ACTION_SHOW,
    ACTION_DIFF,
    ACTION_ADD,
    ACTION_DELETE,
    ACTION_RESOLVE,
    ACTION_START,
    ACTION_STOP
};

inline const String getActionStr(Command &command) {
    return command.getArgument(FPSTR(str_action)).getValue();
}

inline const String getParamStr(Command &command) {
    return command.getArgument(FPSTR(str_param)).getValue();
}

inline const String getIpStr(Command &command) {
    return command.getArgument(FPSTR(str_ip)).getValue();
}

inline const String getMacStr(Command &command) {
    return command.getArgument(FPSTR(str_mac)).getValue();
}

inline const String getModStr(Command &command) {
    return command.getArgument(FPSTR(str_mod)).getValue();
}

inline const String getPathStr(Command &command) {
    return command.getArgument(FPSTR(str_path)).getValue();
}

inline const String getItemStr(Command &command) {
    return command.getArgument(FPSTR(str_item)).getValue();
}

inline const String getValueStr(Command &command) {
    return command.getArgument(FPSTR(str_value)).getValue();
}

inline CommandAction getAction(Command &cmd) {
    String str = getActionStr(cmd);
    if (strcasecmp_P(str.c_str(), str_print) == 0) {
        return ACTION_PRINT;
    } else if (strcasecmp_P(str.c_str(), str_reset) == 0) {
        return ACTION_RESET;
    } else if (strcasecmp_P(str.c_str(), str_save) == 0) {
        return ACTION_SAVE;
    } else if (strcasecmp_P(str.c_str(), str_load) == 0) {
        return ACTION_LOAD;
    } else if (strcasecmp_P(str.c_str(), str_apply) == 0) {
        return ACTION_APPLY;
    } else if (strcasecmp_P(str.c_str(), str_status) == 0) {
        return ACTION_STATUS;
    } else if (strcasecmp_P(str.c_str(), str_log) == 0) {
        return ACTION_LOG;
    } else if (strcasecmp_P(str.c_str(), str_avg) == 0) {
        return ACTION_AVG;
    } else if (strcasecmp_P(str.c_str(), str_on) == 0) {
        return ACTION_ON;
    } else if (strcasecmp_P(str.c_str(), str_off) == 0) {
        return ACTION_OFF;
    } else if (strcasecmp_P(str.c_str(), str_restart) == 0) {
        return ACTION_RESTART;
    } else if (strcasecmp_P(str.c_str(), str_backlight) == 0) {
        return ACTION_BACKLIGHT;
    } else if (strcasecmp_P(str.c_str(), str_uptime) == 0) {
        return ACTION_UPTIME;
    } else if (strcasecmp_P(str.c_str(), str_time) == 0) {
        return ACTION_TIME;
    } else if (strcasecmp_P(str.c_str(), str_wol) == 0) {
        return ACTION_WOL;
    } else if (strcasecmp_P(str.c_str(), str_list) == 0) {
        return ACTION_LIST;
    } else if (strcasecmp_P(str.c_str(), str_clear) == 0) {
        return ACTION_CLEAR;
    } else if (strcasecmp_P(str.c_str(), str_test) == 0) {
        return ACTION_TEST;
    } else if (strcasecmp_P(str.c_str(), str_config) == 0) {
        return ACTION_CONFIG;
    } else if (strcasecmp_P(str.c_str(), str_set) == 0) {
        return ACTION_SET;
    } else if (strcasecmp_P(str.c_str(), str_show) == 0) {
        return ACTION_SHOW;
    } else if (strcasecmp_P(str.c_str(), str_diff) == 0) {
        return ACTION_DIFF;
    } else if (strcasecmp_P(str.c_str(), str_add) == 0) {
        return ACTION_ADD;
    } else if (strcasecmp_P(str.c_str(), str_delete) == 0) {
        return ACTION_DELETE;
    } else if (strcasecmp_P(str.c_str(), str_resolve) == 0) {
        return ACTION_RESOLVE;
    } else if (strcasecmp_P(str.c_str(), str_start) == 0) {
        return ACTION_START;
    } else if (strcasecmp_P(str.c_str(), str_stop) == 0) {
        return ACTION_STOP;
    } else
        return ACTION_UNKNOWN;
}

}
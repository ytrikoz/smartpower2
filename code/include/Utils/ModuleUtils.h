#pragma once

#include "CommonTypes.h"

namespace ModuleUtils {

inline String getStateStr(const ModuleState state) {
    String str;
    switch (state) {
        case STATE_INIT:
        case STATE_INIT_FAILED:
        case STATE_INIT_COMPLETE:
            str = FPSTR(str_init);
            break;
        case STATE_START_FAILED:
            str = FPSTR(str_start);
            break;
        case STATE_ACTIVE:
            str = FPSTR(str_active);
            break;
        default:
            str = FPSTR(str_unknown);
    }
    switch (state) {
        case STATE_INIT_FAILED:
        case STATE_START_FAILED:
            str += ' ';
            str += String(FPSTR(str_failed));
            break;
        case STATE_INIT_COMPLETE:
            str += ' ';
            str += String(FPSTR(str_complete));
            break;
        default:
            break;
    }
    return str;
}

}  // namespace ModuleUtils
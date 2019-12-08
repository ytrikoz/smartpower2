#include "PsuUtils.h"

String getStateStr(PsuState state) {
    return StrUtils::getStrP(state == POWER_ON ? str_on : str_off);
}

String getStatusStr(PsuStatus status) {
    return StrUtils::getStrP(getStatusStrP(status));
}

PGM_P getStatusStrP(PsuStatus status) {
    PGM_P strP;
    switch (status) {
    case PSU_OK:
        strP = str_ok;
        break;
    case PSU_ALERT:
        strP = str_alert;
        break;
    case PSU_ERROR:
        strP = str_error;
        break;
    default:
        strP = str_unknown;
        break;
    }
    return strP;
}

PGM_P getErrorStrP(PsuError value) {
    PGM_P strP;
    switch (value) {
    case PSU_ERROR_LOW_VOLTAGE:
        strP = str_low_voltage;
        break;
    default:
        strP = str_unknown;
        break;
    }
    return strP;
}

PGM_P getAlertStrP(PsuAlert value) {
    PGM_P strP;
    switch (value) {
    case PSU_ALERT_LOAD_LOW:
        strP = str_load_low;
        break;
    default:
        strP = str_unknown;
        break;
    }
    return strP;
}

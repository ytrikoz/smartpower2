#include "PsuUtils.h"

int quadratic_regression(double value, bool alterRange) {
    double a = 0.0000006562;
    double b = 0.0022084236;
    float c = alterRange ? 11.83 : 4.08;
    double d = b * b - a * (c - value);
    double root = (-b + sqrt(d)) / a;
    if (root < 0)
        root = 0;
    else if (root > 255)
        root = 255;
    return root;
}

String getStateStr(const PsuState state) {
    return FPSTR(state == POWER_ON ? str_on : str_off);
}

String getStatusStr(const PsuStatus status) {
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
    }
    return FPSTR(strP);
}

String getErrorStr(const PsuError value) {
    PGM_P strP;
    switch (value) {
        case PSU_ERROR_LOW_VOLTAGE:
            strP = str_low_voltage;
            break;
        default:
            strP = str_unknown;
    }
    return FPSTR(strP);
}

String getAlertStr(const PsuAlert value) {
    PGM_P strP;
    switch (value) {
        case PSU_ALERT_LOAD_LOW:
            strP = str_load_low;
            break;
        default:
            strP = str_unknown;
    }
    return FPSTR(strP);
}

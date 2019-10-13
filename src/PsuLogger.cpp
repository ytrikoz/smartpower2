#include "PsuLogger.h"

#include "PrintUtils.h"

using namespace StrUtils;

PsuLogger::PsuLogger() {
    this->psuLog[static_cast<int>(PsuLogEnum::VOLTAGE)] =
        new PsuLog("V", PSU_LOG_VOLTAGE_SIZE);
    this->psuLog[static_cast<int>(PsuLogEnum::CURRENT)] =
        new PsuLog("I", PSU_LOG_CURRENT_SIZE);
    this->psuLog[static_cast<int>(PsuLogEnum::POWER)] =
        new PsuLog("P", PSU_LOG_POWER_SIZE);
    this->psuLog[static_cast<int>(PsuLogEnum::WATTSHOURS)] =
        new PsuLog("Wh", PSU_LOG_WATTHOURS_SIZE);
}

void PsuLogger::log(PsuInfo &item) {
    if (v_enabled)
        getLog(PsuLogEnum::VOLTAGE)->log(item.time, item.V);
    if (i_enabled)
        getLog(PsuLogEnum::CURRENT)->log(item.time, item.I);
    if (p_enabled)
        getLog(PsuLogEnum::POWER)->log(item.time, item.P);
    if (wh_enabled)
        getLog(PsuLogEnum::WATTSHOURS)->log(item.time, item.mWh / ONE_WATT_mW);

    lastLogged = millis();
}

PsuLog *PsuLogger::getLog(PsuLogEnum item) {
    return psuLog[static_cast<int>(item)];
}

size_t PsuLogger::getSize(PsuLogEnum item) { return getLog(item)->count(); }

bool PsuLogger::getValues(PsuLogEnum item, float *dest, size_t &size) {
    PsuLog *log = getLog(item);
    size = log->count();
    if (size)
        log->values(dest, size);
    return size;
}

void PsuLogger::clear() {
    for (uint8_t i = 0; i < 4; ++i) {
        PsuLog *log = getLog(PsuLogEnum(i));
        log->clear();
    }
    lastLogged = startTime = 0;
    v_enabled = i_enabled = p_enabled = wh_enabled = true;
}

void PsuLogger::print(PsuLogEnum item, Print *p) { getLog(item)->printTo(p); }

void PsuLogger::printDiag(Print *p) {
    if (!lastLogged) {
        p->println(FPSTR(str_empty));
        return;
    }
    p->print('\t');
    p->print(FPSTR(str_count));
    p->print('\t');
    p->print(FPSTR(str_min));
    p->print('\t');
    p->print(FPSTR(str_max));
    p->print('\t');
    p->print(FPSTR(str_avg));
    p->print('\t');
    p->println(FPSTR(str_time));

    for (uint8_t i = 0; i < 4; ++i) {
        PsuLog *log = getLog(PsuLogEnum(i));
        log->printDiag(p);
    }
}

#include "PsuLogger.h"

using StrUtils::formatSize;

PsuLogger::PsuLogger() {
    this->psuLog[VOLTAGE_LOG] = new PsuLog("V", PSU_LOG_VOLTAGE_SIZE);
    this->psuLog[CURRENT_LOG] = new PsuLog("I", PSU_LOG_CURRENT_SIZE);
    this->psuLog[POWER_LOG] = new PsuLog("P", PSU_LOG_POWER_SIZE);
    this->psuLog[WATTSHOURS_LOG] = new PsuLog("Wh", PSU_LOG_WATTHOURS_SIZE);
}

PsuLog *PsuLogger::getLog(PsuLogEnum item) { return psuLog[item]; }

size_t PsuLogger::getSize(PsuLogEnum item) { return getLog(item)->size(); }

bool PsuLogger::getValues(PsuLogEnum item, float *dest, size_t &size) {
    PsuLog *log = getLog(item);
    size = log->count();
    if (size)
        log->values(dest, size);
    return size;
}

void PsuLogger::clear() {
    for (uint8_t i = 0; i < 4; ++i)
        getLog(PsuLogEnum(i))->clear();
    lastUpdated = 0;
    startTime = millis();
    v_enabled = i_enabled = p_enabled = wh_enabled = true;
}

void PsuLogger::log(PsuInfo &item) {
    if (v_enabled)
        this->psuLog[VOLTAGE_LOG]->push(item.time, item.V);
    if (i_enabled)
        this->psuLog[CURRENT_LOG]->push(item.time, item.I);
    if (p_enabled)
        this->psuLog[POWER_LOG]->push(item.time, item.P);
    if (wh_enabled)
        this->psuLog[WATTSHOURS_LOG]->push(item.time, item.mWh / ONE_WATT_mW);
    lastUpdated = millis();
}

void PsuLogger::print(PsuLogEnum item, Print *p) { psuLog[item]->printTo(p); }

void PsuLogger::printDiag(Print *p) {
    if (!lastUpdated) {
        p->println(StrUtils::getStrP(str_empty, false));
        return;
    }
    p->print(StrUtils::getStrP(str_name));
    p->print('\t');
    p->print(StrUtils::getStrP(str_sec));
    p->print('\t');
    p->print(StrUtils::getStrP(str_store));
    p->print('\t');
    p->print(StrUtils::getStrP(str_min));
    p->print('\t');
    p->print(StrUtils::getStrP(str_max));
    p->print('\t');
    p->println(StrUtils::getStrP(str_avg));
    for (uint8_t i = 0; i < 4; ++i)
        getLog(PsuLogEnum(i))->printDiag(p);
}

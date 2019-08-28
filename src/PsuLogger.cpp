#include "PsuLogger.h"

#include <string.h>

#include "CommonTypes.h"
#include "SystemClock.h"

using StrUtils::formatSize;

PsuLog* PsuLogger::getLog(PsuLogItem log) { return psuLog[log]; }

PsuLogger::PsuLogger(Psu* psu) {
    this->psu = psu;
    this->psuLog[VOLTAGE_LOG] = new PsuLog("V", PSU_LOG_VOLTAGE_SIZE);
    this->psuLog[CURRENT_LOG] = new PsuLog("I", PSU_LOG_CURRENT_SIZE);
    this->psuLog[POWER_LOG] = new PsuLog("P", PSU_LOG_POWER_SIZE);
    this->psuLog[WATTSHOURS_LOG] = new PsuLog("Wh", PSU_LOG_WATTHOURS_SIZE);
}

void PsuLogger::begin() {
    for (uint8_t i = 0; i < 4; ++i) getLog(PsuLogItem(i))->clear();
    lastUpdated = 0;
    startTime = millis();
    active = true;
    v_enabled = i_enabled = p_enabled = wh_enabled = true;
}

void PsuLogger::end() { active = false; }

void PsuLogger::log(PsuInfo& item) {
    if (!active) return;
    if (v_enabled) this->psuLog[VOLTAGE_LOG]->push(item.time, item.V);
    if (i_enabled) this->psuLog[CURRENT_LOG]->push(item.time, item.I);
    if (p_enabled) this->psuLog[POWER_LOG]->push(item.time, item.P);
    if (wh_enabled)
        this->psuLog[WATTSHOURS_LOG]->push(item.time, item.mWh / ONE_WATT_mW);
}

void PsuLogger::loop() {
    if (!active) return;
    unsigned long now = millis();
    if (millis_passed(lastUpdated, now) >= PSU_LOG_INTERVAL_ms) {
        PsuInfo pi = psu->getInfo();
        log(pi);
        lastUpdated = now;
    }
}

void PsuLogger::fill(PsuLogItem param, float* array, size_t& size) {
    PsuLog* log = psuLog[param];
    size = log->count();
    log->values(array, size);
}

void PsuLogger::print(Print* p, PsuLogItem param) { psuLog[param]->printTo(p); }

void PsuLogger::printDiag(Print* p) {
    if (!lastUpdated) {
        p->println(getStrP(str_empty, false));
        return;
    }
    p->print(getStrP(str_name));
    p->print('\t');
    p->print(getStrP(str_sec));
    p->print('\t');
    p->print(getStrP(str_store));
    p->print('\t');
    p->print(getStrP(str_min));
    p->print('\t');
    p->print(getStrP(str_max));
    p->print('\t');
    p->print(getStrP(str_avg));
    p->print('\t');
    p->println(getStrP(str_size));
    for (uint8_t i = 0; i < 4; ++i) getLog(PsuLogItem(i))->printDiag(p);
}

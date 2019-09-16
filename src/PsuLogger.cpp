#include "PsuLogger.h"

#include <string.h>

#include "CommonTypes.h"
#include "SystemClock.h"

using StrUtils::formatSize;

PsuLog* PsuLogger::getLog(PsuLogItem logItem) { return psuLog[logItem]; }

bool PsuLogger::getLogValues(PsuLogItem logItem, float* values, size_t& size) {
    PsuLog* log = getLog(logItem);
    size = log->count();
    if (size) log->values(values, size);
    return size;
}

PsuLogger::PsuLogger(Psu* psu) {
    this->psu = psu;
    this->psuLog[VOLTAGE_LOG] = new PsuLog("V", PSU_LOG_VOLTAGE_SIZE);
    this->psuLog[CURRENT_LOG] = new PsuLog("I", PSU_LOG_CURRENT_SIZE);
    this->psuLog[POWER_LOG] = new PsuLog("P", PSU_LOG_POWER_SIZE);
    this->psuLog[WATTSHOURS_LOG] = new PsuLog("Wh", PSU_LOG_WATTHOURS_SIZE);
}

void PsuLogger::start() {
    for (uint8_t i = 0; i < 4; ++i) getLog(PsuLogItem(i))->clear();
    lastUpdated = 0;
    startTime = millis();
    active = true;
    v_enabled = i_enabled = p_enabled = wh_enabled = true;
}

void PsuLogger::stop() { active = false; }

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

void PsuLogger::print(Print* p, PsuLogItem param) { psuLog[param]->printTo(p); }

void PsuLogger::printDiag(Print* p) {
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
    for (uint8_t i = 0; i < 4; ++i) getLog(PsuLogItem(i))->printDiag(p);
}

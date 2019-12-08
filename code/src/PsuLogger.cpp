#include "PsuLogger.h"

#include "PrintUtils.h"

using namespace StrUtils;

MemoryPsuLogger::MemoryPsuLogger() {
    if (PSU_LOG_VOLTAGE_SIZE) {
        this->psuLog[static_cast<int>(PsuLogEnum::VOLTAGE)] =
            new PsuLog("V", PSU_LOG_VOLTAGE_SIZE);
    }
    if (PSU_LOG_CURRENT_SIZE) {
        this->psuLog[static_cast<int>(PsuLogEnum::CURRENT)] =
            new PsuLog("I", PSU_LOG_CURRENT_SIZE);
    }
    if (PSU_LOG_POWER_SIZE) {
        this->psuLog[static_cast<int>(PsuLogEnum::POWER)] =
            new PsuLog("P", PSU_LOG_POWER_SIZE);
    }
    if (PSU_LOG_WATTHOURS_SIZE) {
        this->psuLog[static_cast<int>(PsuLogEnum::WATTSHOURS)] =
            new PsuLog("Wh", PSU_LOG_WATTHOURS_SIZE);
    }
}

void MemoryPsuLogger::log(PsuInfo &item) {
    if (PSU_LOG_VOLTAGE_SIZE)
        getLog(PsuLogEnum::VOLTAGE)->log(item.time, item.V);
    if (PSU_LOG_CURRENT_SIZE)
        getLog(PsuLogEnum::CURRENT)->log(item.time, item.I);
    if (PSU_LOG_POWER_SIZE)
        getLog(PsuLogEnum::POWER)->log(item.time, item.P);
    if (PSU_LOG_WATTHOURS_SIZE)
        getLog(PsuLogEnum::WATTSHOURS)->log(item.time, item.mWh / ONE_WATT_mW);

    lastRecord = millis();
}

PsuLog *MemoryPsuLogger::getLog(PsuLogEnum item) {
    PsuLog *result = 0;
    if (item == PsuLogEnum::VOLTAGE && PSU_LOG_VOLTAGE_SIZE)
        result = psuLog[static_cast<int>(item)];
    if (item == PsuLogEnum::CURRENT && PSU_LOG_CURRENT_SIZE)
        result = psuLog[static_cast<int>(item)];
    if (item == PsuLogEnum::POWER && PSU_LOG_POWER_SIZE)
        result = psuLog[static_cast<int>(item)];
    if (item == PsuLogEnum::WATTSHOURS && PSU_LOG_WATTHOURS_SIZE)
        result = psuLog[static_cast<int>(item)];
    return result;
}

size_t MemoryPsuLogger::getSize(PsuLogEnum item) { 
    return getLog(item)? getLog(item)->count(): 0;
}

bool MemoryPsuLogger::getValues(PsuLogEnum item, float *dest, size_t &size) {
    PsuLog *log = getLog(item);
    if (log) {
        size = log->count();
        if (size)
            log->values(dest, size);
    }
    return size;
}

void MemoryPsuLogger::clear() {
    for (uint8_t i = 0; i < 4; ++i) {
        PsuLog *log = getLog(PsuLogEnum(i));
        log->clear();
    }
    lastRecord = startTime = 0;
}

void MemoryPsuLogger::print(PsuLogEnum item, Print *p) { getLog(item)->printTo(p); }

void MemoryPsuLogger::printDiag(Print *p) {
    if (!lastRecord) {
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

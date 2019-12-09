#include "PsuLogHelper.h"

#include "PrintUtils.h"

using namespace StrUtils;

PsuLogHelper::PsuLogHelper() {
    log_[VOLTAGE] = new DeduplicationLog("V", PSU_LOG_VOLTAGE_SIZE);
    log_[CURRENT] = new DeduplicationLog("I", PSU_LOG_CURRENT_SIZE);
}

void PsuLogHelper::log(PsuData &item) {
    log_[VOLTAGE]->log(item.time, item.V);
    log_[CURRENT]->log(item.time, item.I);
    lastRecord = millis();
}

DeduplicationLog *PsuLogHelper::getLog(PsuLogEnum item) {
    return log_[item];
}

size_t PsuLogHelper::getSize(PsuLogEnum item) { 
    return log_[item] ? getLog(item)->count(): 0;
}

bool PsuLogHelper::getValues(PsuLogEnum item, float *dest, size_t &size) {
    size = log_[item]->count();
    if (size)
        log_[item]->values(dest, size);
    return size;
}

void PsuLogHelper::clear() {
    log_[VOLTAGE]->clear();
    log_[CURRENT]->clear();
    lastRecord = startTime = 0;
}

void PsuLogHelper::print(Print *p, PsuLogEnum item) { 
    log_[item]->printTo(p); 
}

void PsuLogHelper::print(Print *p) {
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

    
    getLog(VOLTAGE)->printDiag(p);
    getLog(CURRENT)->printDiag(p);
}

#include "PowerLog.h"

#include "Utils/PrintUtils.h"

using namespace StrUtils;

PowerLog::PowerLog() {
    log_[VOLTAGE] = new DedupLog("V", PSU_LOG_VOLTAGE_SIZE);
    log_[CURRENT] = new DedupLog("I", PSU_LOG_CURRENT_SIZE);
}

void PowerLog::onPsuData(PsuData &item) {
    log_[VOLTAGE]->log(item.time, item.V);
    log_[CURRENT]->log(item.time, item.I);
    lastRecord_ = millis();
}

DedupLog *PowerLog::getLog(PowerLogEnum item) {
    return log_[item];
}

size_t PowerLog::getSize(PowerLogEnum item) { 
    return log_[item] ? getLog(item)->count(): 0;
}

bool PowerLog::fill(PowerLogEnum item, float *dest, size_t &size) {
    size = log_[item]->count();
    if (size)
        log_[item]->values(dest, size);
    return size;
}

void PowerLog::clear() {
    log_[VOLTAGE]->clear();
    log_[CURRENT]->clear();
    lastRecord_ = startTime = 0;
}

void PowerLog::print(Print *p, PowerLogEnum item) { 
    log_[item]->printTo(p); 
}

void PowerLog::print(Print *p) {
    if (!lastRecord_) {
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

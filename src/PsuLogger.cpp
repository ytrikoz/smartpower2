#include "PsuLogger.h"

#include <string.h>

#include "CommonTypes.h"
#include "TimeUtils.h"

PsuLogger::PsuLogger(Psu* psu, size_t capacity) {
    items = new PsuInfo[capacity];
    this->provider = psu;
    this->capacity = capacity;
    this->writePos = 0;
    this->readPos = 0;
    this->lastTime = 0;
    this->lastItem = 0;
    this->rotated = false;
}

PsuLogger::~PsuLogger() { delete[] items; }

void PsuLogger::begin() {

    clear();
    active = true;
}

void PsuLogger::end() { active = false; }

void PsuLogger::clear() {
    writePos = 0;
    readPos = 0;
    lastTime = 0;
    rotated = false;
}

void PsuLogger::add(PsuInfo item, unsigned long time_ms) {
    if (writePos == capacity) {
        writePos = 0;
        rotated = true;
    }
    items[writePos] = item;
    // items[writePos].time = item.time;
    // items[writePos].current = item.current;
    // items[writePos].power = item.power;
    // items[writePos].voltage = item.voltage;
    // items[writePos].wattSeconds = item.wattSeconds;

    lastItem = item.time;
    writePos++;
    if (rotated) readPos = writePos;
}

void PsuLogger::getVoltages(float* voltages) {
    uint16_t pos = readPos;

    for (int i = 0; i < (int)size(); ++i) {
        if (pos == capacity) pos = 0;
        voltages[i] = items[pos].voltage;
        pos++;
    };
}

void PsuLogger::loop() {
    if (!active) return;

    unsigned long now = millis();
    if (now - lastTime > PSU_LOG_INTERVAL_ms) {
        if (lastTime == 0) {
            lastTime = now;
        } else {
            lastTime += PSU_LOG_INTERVAL_ms;
        }
        PsuInfo info = provider->getInfo();
        add(info, now);
    }
}

bool PsuLogger::empty() { return writePos == 0 && !rotated; }

size_t PsuLogger::size() { return rotated ? capacity : writePos; };

PsuInfo PsuLogger::first() { return items[readPos]; }

PsuInfo PsuLogger::last() {
    size_t pos = readPos;
    if (writePos < readPos) { 
        size_t pos = writePos + 1; 
        if (pos > capacity) pos = 0;
    }    
    return items[pos];
}

void PsuLogger::printFirst(Print* p, uint16_t n) {
    if (n > size()) n = size();
    uint16_t pos = readPos;
    for (int i = 1; i <= n; i++) {
        if (pos == capacity) pos = 0;
        PsuInfo pi = items[pos];
        p->printf("%lu %2.4fV %2.4fA %2.4f", pi.time, pi.voltage, pi.current,
                  pi.power);
        p->println();
        pos++;
    };
}

void PsuLogger::print(Print* p) {
    uint16_t pos = readPos;
    for (size_t i = 1; i <= size(); ++i) {
        if (pos == capacity) pos = 0;
        PsuInfo pi = items[pos];
        p->printf("%d %lu %2.4fV %2.4fA %2.4f", pos, pi.time, pi.voltage,
                  pi.current, pi.power);
        p->println();
        pos++;
    };
}

void PsuLogger::printLast(Print* p, uint16_t n) {
    if (n > writePos) n = writePos;
    for (int i = writePos - n; i < writePos; ++i) {
        PsuInfo pi = items[i];
        p->printf("%lu %2.4fV %2.4fA %2.4fW", millis_passed(pi.time, lastTime),
                  pi.voltage, pi.current, pi.power);
        p->println();
    }
}

void PsuLogger::printDiag(Print* p) {
    p->print(FPSTR(str_psu_log));
    if (empty()) {
        p->println(FPSTR(str_empty));
        return;
    }
    p->print(FPSTR(str_duration));
    unsigned long logDuration =
        floor((float) millis_passed(first().time, last().time) / ONE_SECOND_ms);
    p->printf_P(strf_lu_sec, logDuration);
    p->printf_P(strf_size_d, (int)size());
    p->printf_P(strf_used_per, (float)size() / capacity * 100);
    p->println();
}
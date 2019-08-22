#include "PsuLogger.h"

#include <string.h>

#include "CommonTypes.h"
#include "TimeUtils.h"

PsuLogger::PsuLogger(Psu* psu, size_t capacity) {
    items = new PsuInfo[capacity];
    this->psu = psu;
    this->capacity = capacity;
    clear();
}

PsuLogger::~PsuLogger() { delete[] items; }

void PsuLogger::begin() {
    clear();
    lastUpdated = millis();
    active = true;
}

void PsuLogger::end() { active = false; }

void PsuLogger::clear() {
    writePos = 0;
    rotated = false;
}

void PsuLogger::add(PsuInfo item) {
    items[writePos] = item;
    if (++writePos >= capacity) {
        writePos = 0;
        rotated = true;
    }
}

void PsuLogger::getVoltages(float* voltages) {
    size_t pos = first();
    for (size_t i = 0; i < size(); ++i) {
        voltages[i] = items[pos].voltage;
        pos = next(pos);
    };
}

void PsuLogger::loop() {
    if (!active) return;
    unsigned long now = millis();
    if (millis_passed(lastUpdated, now) >= PSU_LOG_INTERVAL_ms) {
        PsuInfo pi = psu->getInfo();
        add(pi);
        lastUpdated = now;
    }
}

bool PsuLogger::empty() { return writePos == 0 && !rotated; }

size_t PsuLogger::size() { return rotated ? capacity : writePos; };

size_t PsuLogger::first() { return rotated ? next(writePos) : 0; }

size_t PsuLogger::last() { return prev(writePos); }

size_t PsuLogger::next(size_t pos) { return pos + 1 >= capacity ? 0 : ++pos; }

size_t PsuLogger::prev(size_t pos) { return pos == 0 ? pos = capacity - 1 : --pos; }

void PsuLogger::printItem(Print* p, size_t index) {
    PsuInfo pi = items[index];
    p->printf("%lu %2.4fV %2.4fA %2.4fW", pi.time, pi.voltage, pi.current,
              pi.power);
    p->println();
}

void PsuLogger::printFirst(Print* p, size_t num) {
    if (num > size()) num = size();
    size_t pos = first();
    for (size_t i = 0; i < num; ++i) {
        printItem(p, pos);
        pos = next(pos);
    };
}

void PsuLogger::printLast(Print* p, size_t num) {
    if (num > size()) num = size();
    int pos = last();
    for (size_t i = 0; i < num; ++i) {
        printItem(p, pos);
        pos = prev(pos);
    }
}

void PsuLogger::print(Print* p) { printFirst(p, size()); }

void PsuLogger::printDiag(Print* p) {
    if (empty()) {
        p->println(getStrP(str_empty));
        return;
    }
    p->print(getStrP(str_size));
    p->println(size());

    p->printf_P(strf_used_per, (float)size() / capacity * 100);
    p->println();

    p->print(getStrP(str_duration));
    p->printf_P("%.2f ",
                (float)millis_passed(items[first()].time, items[last()].time) /
                    ONE_SECOND_ms);
    p->println(getStrP(str_sec));
}
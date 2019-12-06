#include "PsuLog.h"

#include "StrUtils.h"
#include "Strings.h"

#define UNSET_MAX -16364
#define UNSET_MIN 16364

using namespace StrUtils;

PsuLog::PsuLog(const char *label, size_t size) {
    strcpy(this->name, label);
    items = new LogItem[size]();
    capacity = size;
    clear();
}

void PsuLog::log(const unsigned long time, const float value) {
    if (time > lastTime) {
        lastTime = time;
        counter++;
        updateStats(value, counter);

        uint16_t converted = value;
        if (counter == 1) {
            getEntry()->n = 1;
            getEntry()->v = converted;
            writePos++;
        }

        getEntry()->n = counter;
        getEntry()->v = converted;

        if (converted != getPrevEntry()->v) {
            writePos++;
            getEntry()->n = counter;
            getEntry()->v = converted;
        }
    }
}

void PsuLog::clear() {
    lastTime = counter = writePos = 0;
    items[0].n = 0;
    max = UNSET_MAX;
    min = UNSET_MIN;
    avg = 0;
}

void PsuLog::updateStats(const float val, const size_t count) {
    if (max < val)
        max = val;
    if (min > val)
        min = val;
    avg = ((avg * (count - 1)) + val) / count;
}

LogItem *PsuLog::getEntry() { return getEntry(writePos); }

LogItem *PsuLog::getEntry(size_t pos) { return &items[getEntryIndex(pos)]; }

size_t PsuLog::getEntryIndex(size_t pos) { return pos % capacity; }

LogItem *PsuLog::getPrevEntry() { return &items[getPrevEntryIndex(writePos)]; }

LogItem *PsuLog::getPrevEntry(size_t pos) {
    return &items[getPrevEntryIndex(pos)];
}

size_t PsuLog::getPrevEntryIndex(size_t pos) {
    bool overlaped = pos >= capacity;
    if (overlaped)
        pos = pos % capacity;
    return pos > 0 ? --pos : overlaped ? capacity - 1 : 0;
}

LogItem *PsuLog::getFirstEntry() { return &items[getFirstEntryIndex()]; }

size_t PsuLog::getFirstEntryIndex() {
    return writePos > capacity ? getNextEntryIndex(writePos) : 0;
}

LogItem *PsuLog::getNextEntry(size_t pos) {
    return &items[getNextEntryIndex(pos)];
}

size_t PsuLog::getNextEntryIndex(size_t pos) { return ++pos % capacity; }

LogItem *PsuLog::getLastEntry() { return &items[getLastEntryIndex()]; }

size_t PsuLog::getLastEntryIndex() { return writePos % capacity; }

size_t PsuLog::count() {
    return (writePos > 0) ? getLastEntry()->n - getFirstEntry()->n + 1
                          : items[0].n;
}

void PsuLog::values(float array[], size_t &size) {
    if (size > counter)
        size = counter;
    size_t pos = getFirstEntryIndex();
    for (size_t i = 0; i < size; ++i) {
        LogItem *log = &items[pos];
        array[i] = log->v;
        if (i >= log->n)
            pos = getEntryIndex(pos);
    };
}

void PsuLog::printTo(Print *p) {
    size_t readPos = getFirstEntryIndex();
    size_t first_n = getFirstEntry()->n;
    size_t last_n = getLastEntry()->n;
    if (first_n == last_n)
        first_n = 1;
    for (size_t n = first_n; n <= last_n; ++n) {
        float value = getEntry(readPos)->v;
        if (n >= getEntry(readPos)->n)
            readPos++;
        p->print(n);
        p->print('\t');
        p->println(value, 4);
    }
}

void PsuLog::printDiag(Print *p) {
    p->print(name);
    p->print('\t');
    if (!count()) {
        p->println(FPSTR(str_empty));
        return;
    }
    
    p->print(count());
    p->print('\t');
    p->print(min);
    p->print('\t');
    p->print(max);
    p->print('\t');
    p->print(avg);
    p->print('\t');
    unsigned long period_s =
        floor(count() * PSU_LOG_INTERVAL_ms / ONE_SECOND_ms);
    p->println(getTimeStr(period_s));
}

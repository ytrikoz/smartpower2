#include "Core/DedupLog.h"

#include "Strings.h"
#include "Utils/StrUtils.h"

#define UNSET_MAX -16364
#define UNSET_MIN 16364

using namespace StrUtils;

DedupLog::DedupLog(const char *label, size_t size) {
    strcpy(this->name, label);
    items = new LogItem[size]();
    capacity = size;
    clear();
}

void DedupLog::log(const unsigned long time, const float value) {
    if (time > lastTime) {
        lastTime = time;
        counter++;

        updateStats(value, counter);

        if (counter == 1) {
            entry()->number = 1;
            entry()->value = value;
            writePos++;
        }

        entry()->number = counter;
        entry()->value = value;

        if (value != getPrevEntry()->value) {
            writePos++;
            entry()->number = counter;
            entry()->value = value;
        }
    }
}

void DedupLog::clear() {
    lastTime = 0;
    counter = 0;
    writePos = 0;

    items[0].number = 0;
    max = UNSET_MAX;
    min = UNSET_MIN;
    avg = 0;
}

void DedupLog::updateStats(const float val, const size_t count) {
    if (max < val)
        max = val;
    if (min > val)
        min = val;
    avg = ((avg * (count - 1)) + val) / count;
}

LogItem *DedupLog::entry() { return getEntry(writePos); }

LogItem *DedupLog::getEntry(size_t pos) { return &items[getEntryIndex(pos)]; }

size_t DedupLog::getEntryIndex(size_t pos) { return pos % capacity; }

LogItem *DedupLog::getPrevEntry() { return &items[getPrevEntryIndex(writePos)]; }

LogItem *DedupLog::getPrevEntry(size_t pos) {
    return &items[getPrevEntryIndex(pos)];
}

size_t DedupLog::getPrevEntryIndex(size_t pos) {
    bool overlaped = pos >= capacity;
    if (overlaped)
        pos = pos % capacity;
    return pos > 0 ? --pos : overlaped ? capacity - 1 : 0;
}

LogItem *DedupLog::getFirstEntry() { return &items[getFirstEntryIndex()]; }

size_t DedupLog::getFirstEntryIndex() {
    return writePos > capacity ? getNextEntryIndex(writePos) : 0;
}

LogItem *DedupLog::getNextEntry(size_t pos) {
    return &items[getNextEntryIndex(pos)];
}

size_t DedupLog::getNextEntryIndex(size_t pos) { return ++pos % capacity; }

LogItem *DedupLog::getLastEntry() { return &items[getLastEntryIndex()]; }

size_t DedupLog::getLastEntryIndex() { return writePos % capacity; }

size_t DedupLog::count() {
    return writePos ? getLastEntry()->number - getFirstEntry()->number + 1: items[0].number;
}

void DedupLog::values(float array[], size_t &size) {
    if (size > counter) size = counter;

    size_t pos = getFirstEntryIndex();

    for (size_t i = 0; i < size - 1; ++i) {
        LogItem li = items[pos];
        array[i] = li.value;
        if (i >= li.number) pos = getEntryIndex(pos);
    };
}

void DedupLog::printTo(Print *p) {
    size_t readPos = getFirstEntryIndex();    
    size_t first = getFirstEntry()->number;
    size_t last = getLastEntry()->number;

    if (first == last) first = 1;

    for (size_t n = first; n <= last; ++n) {
        float value = getEntry(readPos)->value;
        if (n >= getEntry(readPos)->number) readPos++;
        p->print(n);
        p->print('\t');
        p->println(value, 4);
    }
}

void DedupLog::printDiag(Print *p) {
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
    p->println(period_s);
}

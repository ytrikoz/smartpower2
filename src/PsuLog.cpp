#include "PsuLog.h"

#define UNSET_MAX -16364
#define UNSET_MIN 16364

using StrUtils::formatSize;

PsuLog::PsuLog(const char* label, size_t size) {
    this->name = new char[strlen(label) + 1];
    strcpy(this->name, label);
    this->capacity = size;
    this->items = new LogItem[capacity]();
    clear();
}

PsuLog::~PsuLog() { delete[] this->name; }

bool PsuLog::available() { return size(); }

void PsuLog::clear() {
    lastTime = 0;
    counter = 0;
    writePos = 0;
    rotated = false;
    value_max = UNSET_MAX;
    value_min = UNSET_MIN;
    value_avg = 0;
}

void PsuLog::calcStat(float value) {
    if (value_max < value) value_max = value;
    if (value_min > value) value_min = value;
}

void PsuLog::push(unsigned long time, float value) {
    if (time == lastTime) return;		
    calcStat(value);
    ++counter;
    lastTime = time;
    if (items[writePos].value == value) {
        items[writePos].n = counter;
        return;
    }
    if (++writePos >= capacity) {
        writePos = 0;
        rotated = true;
    }
    items[writePos].n = counter;
    items[writePos].value = value;
}

LogItem* PsuLog::getFirst() { return &items[first()]; }

LogItem* PsuLog::getLast() { return &items[last()]; }

LogItem* PsuLog::get(size_t pos) { return &items[pos]; }

LogItem* PsuLog::getPrev(size_t pos) { return &items[prev(pos)]; }

LogItem* PsuLog::getNext(size_t pos) { return &items[next(pos)]; }

void PsuLog::values(float array[], size_t& value_max) {
    if (value_max > counter) value_max = counter;
    size_t pos = first();
    for (size_t i = 0; i < value_max; ++i) {
        LogItem log = items[pos];
        array[i] = log.value;
        if (i >= items[pos].n) pos = next(pos);
    };
}

unsigned long PsuLog::duration() { return count() * PSU_LOG_INTERVAL_ms; }

void PsuLog::printTo(Print* p) {
    float value = 0;
    size_t readPos = first();
    size_t first_n = getFirst()->n;
    size_t last_n = getLast()->n;
    if (first_n == last_n) first_n = 1;
    for (size_t n = first_n; n <= last_n; ++n) {
        value = get(readPos)->value;
        if (n >= get(readPos)->n) readPos = next(readPos);
        p->printf("%d\t%.3f", n, value);
        p->println();
    }
}

void PsuLog::printDiag(Print* p) {
    p->print(getStrP(name));
    p->print('\t');
    if (!available()) {
        p->println(getStrP(str_empty, false));
        return;
    }
    size_t stored_n = getLast()->n - getFirst()->n;
    p->print(
        String((float)(stored_n * PSU_LOG_INTERVAL_ms) / ONE_SECOND_ms, 0));
    p->print('\t');
    p->print(count());
    p->print('\t');
    p->printf("%.3f\t%.3f\t%.3f\t", value_min, value_max, value_avg);
    p->println(formatSize(size() * sizeof(LogItem)));
}
size_t PsuLog::size() { return rotated ? capacity : writePos; }

size_t PsuLog::count() {
    if (!available()) return 0;
    if (size() > 1) {
        return (getLast()->n - getFirst()->n) + 1;
    } else {
        return counter;
    }
}

size_t PsuLog::free() { return rotated ? 0 : capacity - writePos; }

size_t PsuLog::first() { return rotated ? next(writePos) : 0; }

size_t PsuLog::last() { return writePos > 0 ? prev(writePos) : 0; }

size_t PsuLog::prev(size_t pos) { return (pos == 0) ? capacity - 1 : --pos; }

size_t PsuLog::next(size_t pos) { return pos + 1 >= capacity ? 0 : ++pos; }

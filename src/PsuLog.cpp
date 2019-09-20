#include "PsuLog.h"

#define UNSET_MAX -16364
#define UNSET_MIN 16364

PsuLog::PsuLog(const char* label, size_t size) {
    this->name = new char[strlen(label) + 1];
    strcpy(this->name, label);
    this->capacity = size;
    this->items = new LogItem[capacity]();
    clear();
}

PsuLog::~PsuLog() { delete[] this->name; }

void PsuLog::clear() {
    lastTime = counter = writePos = 0;
    items[0].n = 0;
    value_max = UNSET_MAX;
    value_min = UNSET_MIN;
    value_avg = 0;
}

size_t PsuLog::write(const size_t n, const float val) {
    getItem()->n = n;
    getItem()->value = (val);
    return writePos;
}

void PsuLog::pushItem(const size_t n, const float val) {
    uint16_t cval = convert(val);
    if (n == 1) {
        write(n, cval);
        writePos++;
    }
    size_t pos = write(n, cval);
    if (getItem(pos)->value != getPrev(pos)->value) {
        writePos++;
        write(n, cval);
    }
}

void PsuLog::calcMinMaxAvg(const float val, const size_t cnt) {
    if (value_max < val) value_max = val;
    if (value_min > val) value_min = val;
    value_avg = ((value_avg * (cnt - 1)) + val) / cnt;
}

float PsuLog::revert(uint16_t value) {
    return (float) value  / 1000;
}
uint16_t PsuLog::convert(float value) { return floor(value * 1000); }

void PsuLog::push(const unsigned long time, const float value) {
    if (time > lastTime) {
        lastTime = time;
        counter++;
        calcMinMaxAvg(value, counter);
        pushItem(counter, value);
    }
}

size_t PsuLog::getItemIndex(size_t pos) { return pos % capacity; }

LogItem* PsuLog::getItem() { return &items[getItemIndex(writePos)]; }

LogItem* PsuLog::getItem(size_t pos) { return &items[getItemIndex(pos)]; }

LogItem* PsuLog::getNext(size_t pos) { return &items[next(pos)]; }

LogItem* PsuLog::getPrev(size_t pos) { return &items[prev(pos)]; }

LogItem* PsuLog::getFirst() { return &items[first()]; }

LogItem* PsuLog::getLast() { return &items[last()]; }

LogItem* PsuLog::getPrev() { return &items[prev(writePos)]; }

size_t PsuLog::first() { return writePos > capacity ? next(writePos) : 0; }

size_t PsuLog::last() { return writePos % capacity; }

size_t PsuLog::prev(size_t pos) {
    bool overlaped = pos >= capacity;
    if (overlaped) pos = pos % capacity;
    return pos > 0 ? --pos : overlaped ? capacity - 1 : 0;
}

size_t PsuLog::next(size_t pos) { return ++pos % capacity; }

bool PsuLog::available() { return size(); }

size_t PsuLog::size() { return count(); }

size_t PsuLog::count() {
    return (writePos > 0) ? getLast()->n - getFirst()->n + 1 : items[0].n;
}

void PsuLog::values(float array[], size_t& array_size) {
    if (array_size > counter) array_size = counter;
    size_t pos = first();
    for (size_t i = 0; i < array_size; ++i) {
        LogItem* log = &items[pos];
        array[i] = revert(log->value);
        if (i >= log->n) pos = next(pos);
    };
}

void PsuLog::printTo(Print* p) {
    float value = 0;
    size_t readPos = first();
    size_t first_n = getFirst()->n;
    size_t last_n = getLast()->n;
    if (first_n == last_n) first_n = 1;
    for (size_t n = first_n; n <= last_n; ++n) {
        value = getItem(readPos)->value;
        if (n >= getItem(readPos)->n) readPos++;
        p->printf("%d\t%.3f", n, value);
        p->println();
    }
}

void PsuLog::printDiag(Print* p) {
    p->print(StrUtils::getStrP(name));
    p->print('\t');
    if (!available()) {
        p->println(StrUtils::getStrP(str_empty, false));
    } else {
        unsigned long interval_s =
            floor(count() * PSU_LOG_INTERVAL_ms / ONE_SECOND_ms);
        p->printf("%lu\t%d\t%.3f\t%.3f\t%.3f\r\n", interval_s, count(),
                  value_min, value_max, value_avg);
    };
}

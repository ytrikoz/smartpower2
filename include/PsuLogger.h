#pragma once

#include <string.h>
#include "Types.h"
#include "consts.h"

class PsuLogger {
   public:
    PsuLogger(Psu* provider, size_t capacity);
    ~PsuLogger();
    bool empty();
    size_t length();
    size_t size();
    void clear();
    void loop();
    void printFirst(uint16_t n);
    void printLast(uint16_t n);
    void printSummary();

   private:
    void add(PsuInfo item, unsigned long time_ms);
    Psu* provider;
    PsuInfo* items;
    uint16_t capacity;
    uint16_t writePos;
    uint16_t readPos;
    unsigned long lastTime;
    unsigned long lastItem;
    bool overlaped;
};

inline PsuLogger::PsuLogger(Psu* provider, size_t capacity) {
    this->capacity = capacity;
    this->provider = provider;

    items = new PsuInfo[capacity];

    writePos = 0;
    readPos = 0;
    lastTime = 0;
    lastItem = 0;
    overlaped = false;
}

inline PsuLogger::~PsuLogger() { delete[] items; }

inline void PsuLogger::clear() {
    memset(items, 0, sizeof(PsuInfo) * capacity);
    writePos = 0;
    readPos = 0;
    overlaped = false;
}

inline void PsuLogger::add(PsuInfo item, unsigned long time_ms) {
    if (writePos == capacity) {
        writePos = 0;
        overlaped = true;
    }
    PsuInfo* log = &items[writePos];
    log->time = time_ms;
    log->current = item.current;
    log->power = item.power;
    log->voltage = item.voltage;
    log->wattSeconds = item.wattSeconds;

    lastItem = item.time;
    writePos++;
    if (overlaped) readPos = writePos;
}

inline void PsuLogger::loop() {
    unsigned long now = millis();
    if (now - lastTime > PSU_LOG_INTERVAL_ms) {
        if (lastTime == 0) { lastTime = now; } else { lastTime += PSU_LOG_INTERVAL_ms;}
        PsuInfo info = provider->getInfo();
        add(info, now);
    }
}

inline void PsuLogger::printSummary() {
    USE_SERIAL.print(FPSTR(str_psu));
    USE_SERIAL.print("log size ");
    USE_SERIAL.print(this->size());
    USE_SERIAL.println();
}

inline void PsuLogger::printFirst(uint16_t n) {
    if (n > size()) n = size();
    uint16_t pos = readPos;
    for (int i = 1; i <= n; i++) {
        if (pos == capacity) pos = 0;
        PsuInfo pi = items[pos];
        USE_SERIAL.printf("%lu %2.4fV %2.4fA %2.4fP", pi.time, pi.voltage,
                          pi.current, pi.power);
        USE_SERIAL.println();
        pos++;
    };
}

inline void PsuLogger::printLast(uint16_t n) {
    if (n > writePos) n = writePos;
    for (int i = writePos - n; i < writePos; i++) {
        PsuInfo pi = items[i];
        USE_SERIAL.printf("%lu %2.4fV %2.4fA %2.4fP", pi.time, pi.voltage,
                          pi.current, pi.power);
        USE_SERIAL.println();
    }
}

inline bool PsuLogger::empty() { return (writePos == 0); }

inline size_t PsuLogger::size() { return overlaped ? capacity : writePos; };
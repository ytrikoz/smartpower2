#include "PsuLogger.h"

PsuLogger::PsuLogger(Psu* provider, size_t capacity) {
    this->items = new PsuInfo[capacity];
    this->provider = provider;
    this->capacity = capacity;
    this->writePos = 0;
    this->readPos = 0;
    this->lastTime = 0;
    this->lastItem = 0;
    this->full = false;
}

PsuLogger::~PsuLogger() { delete[] items; }

void PsuLogger::begin() {
    clear();
    active = true;
}

void PsuLogger::end() { active = false; }

void PsuLogger::clear() {
    memset(items, 0, sizeof(PsuInfo) * capacity);
    writePos = 0;
    readPos = 0;
    full = false;
}

void PsuLogger::add(PsuInfo item, unsigned long time_ms) {
    if (writePos == capacity) {
        writePos = 0;
        full = true;
    }
    items[writePos].time = time_ms;
    items[writePos].current = item.current;
    items[writePos].power = item.power;
    items[writePos].voltage = item.voltage;
    items[writePos].wattSeconds = item.wattSeconds;

    lastItem = item.time;
    writePos++;
    if (full) readPos = writePos;
}

void PsuLogger::getVoltages(float* voltages) {
    uint16_t pos = readPos;

    for (int i = 0; i < (int) size(); i++) {
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

void PsuLogger::printFirst(uint16_t n) {
    if (n > size()) n = size();
    uint16_t pos = readPos;
    for (int i = 1; i <= n; i++) {
        if (pos == capacity) pos = 0;
        PsuInfo pi = items[pos];
        USE_SERIAL.printf("%lu %2.4fV %2.4fA %2.4f", pi.time, pi.voltage,
                          pi.current, pi.power);
        USE_SERIAL.println();
        pos++;
    };
}

void PsuLogger::printLast(uint16_t n) {
    if (n > writePos) n = writePos;
    for (int i = writePos - n; i < writePos; i++) {
        PsuInfo pi = items[i];
        USE_SERIAL.printf("%lu %2.4fV %2.4fA %2.4fW", pi.time, pi.voltage,
                          pi.current, pi.power);
        USE_SERIAL.println();
    }
}

bool PsuLogger::empty() { return writePos == 0 && !full; }

size_t PsuLogger::size() { return full ? capacity : writePos; };

void PsuLogger::printDiag(Print* p) {
    p->print(FPSTR(str_psu));
    p->print(provider->getStateDescription());
    if (provider->getState() == POWER_ON) {
        p->printf_P(strf_for_lu_ms, provider->getDuration());
    }
    p->println();

    p->print(FPSTR(str_psu));
    p->printf_P(strf_output_voltage, provider->getOutputVoltage());
    p->print(FPSTR(str_log));
    p->print(FPSTR(str_size));
    p->print(size());
    p->println();
}

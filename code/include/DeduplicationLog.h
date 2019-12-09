#pragma once

#include "Arduino.h"

struct LogItem {
    size_t number;
    double value;
};

class DeduplicationLog {
   public:
    void log(unsigned long time, float value);
    void values(float array[], size_t &size);

   public:
    DeduplicationLog(const char *label, size_t size);

    void printTo(Print *p);
    void printFirst(Print *p, size_t n);
    void printLast(Print *p, size_t n);
    void printDiag(Print *p);
    void clear();
    size_t count();

   private:
    LogItem *entry();
    LogItem *getPrevEntry();
    LogItem *getFirstEntry();
    LogItem *getLastEntry();

   private:
    LogItem *getEntry(size_t pos);
    LogItem *getPrevEntry(size_t pos);
    LogItem *getNextEntry(size_t pos);

   private:
    size_t getEntryIndex(size_t pos);
    size_t getPrevEntryIndex(size_t pos);
    size_t getNextEntryIndex(size_t pos);
    size_t getFirstEntryIndex();
    size_t getLastEntryIndex();

   private:
    void updateStats(const float value, const size_t cnt);
    void print(Print *p, size_t n, float value);

   private:
    char name[8];
    float min, max, avg;
    unsigned long lastTime;
    size_t counter, capacity, writePos;
    LogItem *items;
};

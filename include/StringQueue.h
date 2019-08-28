#pragma once

#include <Arduino.h>

#include "Strings.h"

class StringQueue : public Printable {
    enum QueueState { QS_PUT, QS_GET };

   public:
    StringQueue(size_t capacity = 64);
    void clear();
    bool available();
    size_t free();
    void put(String& str);
    void get(String& str);
    // Printable
   public:
    size_t printTo(Print& p) const;

   private:
    void preparePut();
    void prepareGet();
    QueueState state;
    size_t capacity;
    std::vector<String> items;
#ifdef DEBUG_FILE_STORAGE
    Print* dbg = &DEBUG;
#endif
};
#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "Core/CircularBuffer.h"
#include "Core/CharBuffer.h"
#include "Strings.h"

class Logger : public Print {
   public:
    Logger(){};

    void loop() {
        AppLogItem buf;
        if (pool_.pop(buf))
            Serial.print(buf.str);
    }

    size_t write(uint8_t ch) override {
        if (ch == '\n') ch = '\x00';
        buffer_.write(ch);
        if (ch == '\x00')
            push(buffer_.c_str());
        buffer_.clear();
        return 1;
    }

    size_t write(const uint8_t* buffer, size_t size) override {
        buffer_.write(buffer, size);
        push(buffer_.c_str());
        buffer_.clear();
        return size;
    }

    size_t onDiag(Print* p) {
        DynamicJsonDocument doc(128);
        doc[FPSTR(str_size)] = pool_.size();
        doc[FPSTR(str_full)] = pool_.full();
        return serializeJsonPretty(doc, *p);
    }

   private:
    void push(const char* str) {
        AppLogItem item;
        item.str = String(str);
        push(item);
    }

    void push(const AppLogItem& item) {
        pool_.push(item);
    }

   private:
    CharBuffer buffer_;
    CircularBuffer<AppLogItem, 128> pool_;
};

#pragma once

#include <Arduino.h>

#include "Core/CircularBuffer.h"
#include "Core/CharBuffer.h"
#include "Core/Logger.h"

#include "Strings.h"

class MainLog : public StringPullable, public StringPusher, public Print {
   public:
    MainLog() : out_(nullptr) {
        buffer_ = new CharBuffer(128);
    };

    virtual size_t write(uint8_t ch) override {
        Serial.print((char)ch);
        if (ch == '\n') {
            push(buffer_->c_str());
            buffer_->clear();
            return 1;
        }
        if (!buffer_->free()) {
            push(buffer_->c_str());
            buffer_->clear();
        }
        return buffer_->write(ch);
    }

    virtual bool pull(String& item) {
        bool res = pool_.pop(item);
        if (out_ != nullptr) {
            out_->print(item);
        }
        return res;
    }

   protected:
    virtual bool push(const String& item) override {
        pool_.push(item);
        return true;
    }

   private:
    Print* out_;
    CharBuffer* buffer_;
    CircularBuffer<String, 32> pool_;
};

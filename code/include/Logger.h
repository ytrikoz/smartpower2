#pragma once

#include <Arduino.h>

#include "Strings.h"
#include "Core/CircularBuffer.h"
#include "Core/CharBuffer.h"

class Logger : public Print {
   public:
    Logger(){
        out_ = nullptr;
        buffer_ = new CharBuffer(128);
    };

    void setOutput(Print* p) {
        out_ = p;
    }

    void loop() {
        String buf;
        if (pool_.pop(buf)) {
            if (out_ != nullptr) 
                out_->print(buf);           
        }
    }

    size_t write(uint8_t ch) override {    
        Serial.print((char)ch);
        if (ch == '\n') {
            push();      
            return 1;
        }        
        if (!buffer_->free()) {
            push();            
        }
        return buffer_->write(ch);        
    }

    size_t onDiag(Print* p) {
        DynamicJsonDocument doc(128);
        doc[FPSTR(str_size)] = pool_.size();
        doc[FPSTR(str_full)] = pool_.full();
        return serializeJsonPretty(doc, *p);
    }

private:
    void push() {
        push(buffer_->c_str());
        buffer_->clear();        
    }
    
    void push(const String& item) {
        pool_.push(item);
    }
    
   private:
    Print* out_;
    CharBuffer* buffer_;
    CircularBuffer<String, 32> pool_;
};

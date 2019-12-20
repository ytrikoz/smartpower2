#pragma once

#include <Arduino.h>

class CharBuffer : Print {
   public:
    CharBuffer(size_t size) : capacity_(size < 2 ? 2 : size), write_(0), read_(0) {
        pool_ = new char[capacity_ + 1];
        memset(pool_, 0, capacity_ + 1);
    }

    CharBuffer(const CharBuffer &src) {
        capacity_ = src.capacity_;
        write_ = src.write_;
        memcpy(pool_, src.pool_, src.write_);
    }

    CharBuffer(const char *str) : CharBuffer(strlen(str) + 1) {
        write((const uint8_t *)str, strlen(str));
    }

    ~CharBuffer() {
        delete pool_;
    }

    void clear() {
        memset(pool_, 0, capacity_);
        write_ = 0;
        read_ = 0;
    }

    size_t size() const { return capacity_; }

    size_t free() const { return capacity_ - write_ - 2; }

    size_t available() const { return write_; }

    const char *c_str() {         
        if (pool_[write_] != '\x00') 
            pool_[write_] = '\x00';            
        return pool_; 
    }

    size_t write(char ch) {
        return write((uint8_t)ch);
    };

    size_t write(const uint8_t ch) {
        size_t n = 0;
        if (write_ < (capacity_ - 2)) {
            pool_[write_++] = ch;
            n = 1;
        }
        return n;
    }

    size_t write(const uint8_t *ptr, const size_t size) {
        size_t n = 0;
        while (n < size) {
            uint8_t ch = ptr[n++];
            if (!write(ch))
                break;
        }
        return n;
    }

   protected:

    char *pool_;
    size_t capacity_;
    size_t write_;
    size_t read_;
};

#pragma once

#include <Arduino.h>

class CharBuffer : Print {
   public:
    CharBuffer(size_t capacity);
    CharBuffer(const CharBuffer &b);
    ~CharBuffer();
    virtual void flush(){};
    virtual size_t first();
    virtual size_t last();
    virtual size_t free();
    virtual size_t size();
    virtual size_t available();
    virtual void clear();
    virtual void resize(size_t size);
    char *c_str();

   public:
    size_t write(char ch);
    virtual size_t write(const uint8_t *buffer, size_t size);
    virtual size_t write(uint8_t byte);

   protected:
    size_t capacity;
    char *buf;
    size_t writePos;
    size_t readPos;
};

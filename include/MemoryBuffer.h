#pragma once

#include <Stream.h>

class MemoryBuffer : public Stream {
  public:
    static const size_t DEFALT_CAPACITY = 128;
    MemoryBuffer(size_t capacity = MemoryBuffer::DEFALT_CAPACITY);
    ~MemoryBuffer();
    void clear();
    virtual size_t write(uint8_t);
    virtual int availableForWrite(void);
    virtual int available();
    virtual int read();
    virtual int peek();
    virtual void flush();

  private:
    uint8_t *buffer;
    size_t capacity;
    size_t pos, size;
};
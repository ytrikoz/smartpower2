 #pragma once

#include <Stream.h>

class MemoryBuffer : public Stream {
  public:
    static const size_t DEFALT_CAPACITY = 128;
    MemoryBuffer(size_t capacity = MemoryBuffer::DEFALT_CAPACITY);
    ~MemoryBuffer();
    void clear();
    int availableForWrite(void);

    virtual size_t write(uint8_t) override;
    virtual int available() override;
    virtual int read() override;
    virtual int peek() override;
    virtual void flush() override;

  private:
    uint8_t *buffer;
    size_t capacity;
    size_t pos, size;
};
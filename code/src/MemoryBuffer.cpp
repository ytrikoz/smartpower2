#include "MemoryBuffer.h"

MemoryBuffer::MemoryBuffer(size_t capacity) {
    buffer = (uint8_t *)malloc(capacity);
    this->capacity = capacity;
    pos = 0;
    size = 0;
}

MemoryBuffer::~MemoryBuffer() { free(buffer); }

void MemoryBuffer::clear() {
    pos = 0;
    size = 0;
}

int MemoryBuffer::read() {
    if (size == 0) {
        return -1;
    } else {
        int ret = buffer[pos];
        pos++;
        size--;
        if (pos == capacity) {
            pos = 0;
        }
        return ret;
    }
}

size_t MemoryBuffer::write(uint8_t value) {
    if (size == capacity) {
        return 0;
    } else {
        size_t p = pos + size;
        if (p >= capacity) {
            p -= capacity;
        }
        buffer[p] = value;
        size++;
        return 1;
    }
}

int MemoryBuffer::available() { return size; }

int MemoryBuffer::availableForWrite() { return capacity - size; }

int MemoryBuffer::peek() { return size == 0 ? -1 : buffer[pos]; }

void MemoryBuffer::flush() {}

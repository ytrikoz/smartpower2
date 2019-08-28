#include "Buffer.h"

Buffer::Buffer(size_t size) {
    if (size < 2) size = 2;
    buf = new char[size];
    capacity = size;
    clear();
}

Buffer::Buffer(const Buffer& b) {
    capacity = b.capacity;
    writePos = b.writePos;
    memcpy(buf, b.buf, writePos);
}

Buffer::~Buffer() { delete[] buf; }

void Buffer::resize(size_t size) {
    char* tmp = buf;
    buf = new char[size];
    capacity = size;
    writePos = writePos > size ? size : writePos;
    readPos = readPos > size ? size : readPos;
    memcpy(buf, tmp, capacity);
    delete[] tmp;
}

size_t Buffer::write(char ch) { return write((uint8_t) ch);};

size_t Buffer::write(uint8_t byte) {
    // Full
    if (writePos >= free() - 1) return 0;
    buf[writePos++] = byte;
    return 1;
}

size_t Buffer::write(const uint8_t* buffer, size_t size) {
    size_t n = 0;
    while (n < size) {
        uint8_t ch = buffer[n++];
        if (!write(ch)) break;
    }
    if (buf[writePos] != '\x00') buf[++writePos] = '\x00';
    return n;
}

void Buffer::clear() {
    memset(buf, 0, capacity * sizeof(buf[0]));
    for (unsigned int i = 0; i < capacity; i++) buf[i] = '\x00';
    writePos = 0;
    readPos = 0;
}

char* Buffer::c_str() { return buf; }

size_t Buffer::size() { return capacity; }

size_t Buffer::free() { return capacity - writePos; }

size_t Buffer::first() { return 0; }

size_t Buffer::last() { return writePos; }

size_t Buffer::available() { return writePos; }

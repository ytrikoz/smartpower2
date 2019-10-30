#include "CharBuffer.h"

CharBuffer::CharBuffer(size_t size) {
    if (size < 2)
        size = 2;
    buf = new char[size];
    capacity = size;
    clear();
}

CharBuffer::CharBuffer(const CharBuffer &b) {
    capacity = b.capacity;
    writePos = b.writePos;
    memcpy(buf, b.buf, writePos);
}

CharBuffer::~CharBuffer() { delete[] buf; }

void CharBuffer::resize(size_t size) {
    char *tmp = buf;
    buf = new char[size];
    capacity = size;
    writePos = writePos > size ? size : writePos;
    readPos = readPos > size ? size : readPos;
    memcpy(buf, tmp, capacity);
    delete[] tmp;
}

size_t CharBuffer::write(char ch) { return write((uint8_t)ch); };

size_t CharBuffer::write(uint8_t byte) {
    // Full
    if (writePos >= free() - 1)
        return 0;
    buf[writePos++] = byte;
    return 1;
}

size_t CharBuffer::write(const uint8_t *CharBuffer, size_t size) {
    size_t n = 0;
    while (n < size) {
        uint8_t ch = CharBuffer[n++];
        if (!write(ch))
            break;
    }
    if (buf[writePos] != '\x00')
        buf[++writePos] = '\x00';
    return n;
}

void CharBuffer::clear() {
    memset(buf, 0, capacity * sizeof(buf[0]));
    for (unsigned int i = 0; i < capacity; i++)
        buf[i] = '\x00';
    writePos = 0;
    readPos = 0;
}

char *CharBuffer::c_str() { return buf; }

size_t CharBuffer::size() { return capacity; }

size_t CharBuffer::free() { return capacity - writePos; }

size_t CharBuffer::first() { return 0; }

size_t CharBuffer::last() { return writePos; }

size_t CharBuffer::available() { return writePos; }

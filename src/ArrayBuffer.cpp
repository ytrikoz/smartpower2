#include "ArrayBuffer.h"

ArrayBuffer::ArrayBuffer(size_t size) : Buffer(size) {}

ArrayBuffer::ArrayBuffer(const Buffer& b) : Buffer(b) {}

char& ArrayBuffer::operator[](unsigned int i) { return buf[i]; }

char ArrayBuffer::operator[](unsigned int i) const { return buf[i]; }

ArrayBuffer& ArrayBuffer::operator=(const ArrayBuffer& b) {
    delete[] buf;
    readPos = b.readPos;
    writePos = b.writePos;
    buf = new char[b.capacity];
    memcpy(buf, b.buf, b.capacity);
    return (*this);
}

// size_t ArrayBuffer::write(const char* str) {
//     size_t len = strlen(str);
//     if (len > capacity - 2) len = capacity - 2;
//     strncpy(buf, str, len);
//     writePos += len;
//     buf[len] = '\x0';
//     return writePos;
// }

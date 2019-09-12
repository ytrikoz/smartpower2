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
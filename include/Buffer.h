#pragma once

#include <string.h>

class Buffer {
   public:
    Buffer(size_t capacity = 64);
    Buffer(const Buffer& b);
    Buffer& operator=(const Buffer& b);
    char& operator[](unsigned int i);
    char operator[](unsigned int i) const;
    ~Buffer();
    bool empty();
    size_t length();
    size_t size();
    void clear();
    void insert(const char ch);
    void set(const char* ch);
    void set(Buffer* buf);
    char* c_str();
    void next();
    void prev();
    void first();
    void last();
    void backsp();
    void del();
    void resize(size_t size);

   private:
    size_t capacity;
    char* buf;
    size_t writePos;
    size_t readPos;
};

inline Buffer::Buffer(size_t size) : capacity(size), buf(new char[capacity]) {
    clear();
}
inline Buffer::Buffer(const Buffer& b)
    : capacity(b.capacity), writePos(b.writePos), readPos(b.readPos) {
    buf = new char[b.capacity];
    memcpy(buf, b.buf, writePos);
}

inline void Buffer::resize(size_t size) {
    char* tmp = buf;

    buf = new char[size];
    capacity = size;
    writePos = writePos > size ? size : writePos;
    readPos = readPos > size ? size : readPos;

    memcpy(buf, tmp, capacity);

    delete[] tmp;
}

inline Buffer::~Buffer() { delete[] buf; }

inline Buffer& Buffer::operator=(const Buffer& b) {
    delete[] buf;
    readPos = b.readPos;
    writePos = b.writePos;
    buf = new char[b.capacity];
    memcpy(buf, b.buf, b.capacity);
    return (*this);
}

inline void Buffer::set(const char* str) {
    size_t len = strlen(str);
    if (len > capacity - 2) len = capacity - 2;
    strncpy(buf, str, len);
    buf[len] = '\x00';
    writePos = len;
}

inline size_t Buffer::length() { return writePos; }

inline size_t Buffer::size() { return this->capacity; };

inline void Buffer::insert(const char ch) {
    if (writePos >= capacity - 2) return;
    for (unsigned int i = capacity; i > writePos; i--) buf[i] = buf[i - 1];
    buf[writePos] = ch;
    writePos++;
}

inline void Buffer::prev() {
    if (writePos > 0) writePos--;
}

inline void Buffer::next() {
    if (writePos < capacity - 1) writePos++;
}

inline void Buffer::first() { writePos = 0; }

inline void Buffer::last() { writePos = strlen(buf); }

inline void Buffer::backsp() {
    if (writePos == 0) return;
    writePos--;
    for (unsigned int i = writePos; i < capacity; i++) buf[i] = buf[i + 1];
    buf[writePos] = '\x00';
}

inline void Buffer::del() {
    if (writePos > capacity - 1) return;
    writePos--;
    for (unsigned int i = writePos; i < capacity; i++) buf[i] = buf[i + 1];
    buf[writePos] = '\x00';
}

inline void Buffer::clear() {
    for (unsigned int i = 0; i < capacity; i++) buf[i] = '\x00';
    writePos = 0;
    readPos = 0;
}

inline bool Buffer::empty() { return (writePos == 0); }

inline char* Buffer::c_str() { return buf; }

inline char& Buffer::operator[](unsigned int i) { return buf[i]; }

inline char Buffer::operator[](unsigned int i) const { return buf[i]; }

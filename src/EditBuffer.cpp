#include "EditBuffer.h"

EditBuffer::EditBuffer(size_t size) : ArrayBuffer(size){};

EditBuffer::EditBuffer(const char *str) : ArrayBuffer(strlen(str) + 1) {
    clear();
    write((const uint8_t *)str, strlen(str));
}

bool EditBuffer::backspace() {
    if (prev()) {
        del();
        return true;
    }
    return false;
}

void EditBuffer::del() {
    for (size_t pos = writePos; pos < capacity; ++pos)
        buf[pos] = buf[pos + 1];
    buf[writePos] = '\x00';
}

bool EditBuffer::next() {
    if (writePos < capacity - 1) {
        ++writePos;
        return true;
    }
    return false;
}

bool EditBuffer::prev() {
    if (writePos > 0) {
        --writePos;
        return true;
    }
    return false;
}

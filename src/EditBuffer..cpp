#include "EditBuffer.h"

EditBuffer::EditBuffer(size_t size) : ArrayBuffer(size){};

EditBuffer::EditBuffer(const char *str) : ArrayBuffer(strlen(str) + 1) {
    clear();
    write((const uint8_t *)str, strlen(str));
}

void EditBuffer::onBackspace() {
    prev();
    onDelete();
}

void EditBuffer::onDelete() {
    for (size_t pos = writePos; pos < capacity; pos++) buf[pos] = buf[pos + 1];
    buf[writePos] = '\x00';
}

void EditBuffer::next() {
    if (writePos < capacity - 1) ++writePos;
}

void EditBuffer::prev() {
    if (writePos > 0) --writePos;
}

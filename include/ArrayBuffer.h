#pragma once

#include <Print.h>
#include <string.h>

#include "CharBuffer.h"

class ArrayBuffer : public CharBuffer {
  public:
    ArrayBuffer(size_t size);
    ArrayBuffer(const CharBuffer &b);
    ArrayBuffer &operator=(const ArrayBuffer &b);
    char &operator[](unsigned int i);
    char operator[](unsigned int i) const;
};

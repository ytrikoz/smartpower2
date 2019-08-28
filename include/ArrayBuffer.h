#pragma once

#include <Print.h>
#include <string.h>

#include "Buffer.h"

class ArrayBuffer : public Buffer {
   public:
    ArrayBuffer(size_t size);
    ArrayBuffer(const Buffer& b);
    ArrayBuffer& operator=(const ArrayBuffer& b);
    char& operator[](unsigned int i);
    char operator[](unsigned int i) const;
};

#pragma once

#include <Arduino.h>

#include "ArrayBuffer.h"

class EditBuffer : public ArrayBuffer {
   public:
    EditBuffer(size_t size);
    EditBuffer(const char *str);

   public:
    bool next();
    bool prev();
    bool backspace();

   protected:
    void del();
};
#pragma once

#include <Arduino.h>

#include "ArrayBuffer.h"

class EditBuffer : public ArrayBuffer {
   public:
    EditBuffer(size_t size);
    EditBuffer(const char *str);
   public:
    virtual void next();
    virtual void prev();
    virtual void onDelete();
    virtual void onBackspace();
};
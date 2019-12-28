#pragma once

#include "Core/CharBuffer.h"

class EditLine : public CharBuffer {
   public:
    EditLine(size_t size) : CharBuffer(size){};

    char &operator[](size_t i) { return pool_[i]; }

    char operator[](size_t i) const { return pool_[i]; }

    EditLine &operator=(const EditLine &src) {
        delete[] pool_;
        pool_ = new char[src.capacity_];
        memcpy(pool_, src.pool_, src.capacity_);
        read_ = src.read_;
        write_ = src.write_;
        return *this;
    }

    void del() {
        size_t i;
        for (i = write_; i < capacity_; ++i)
            pool_[i] = pool_[i + 1];
        pool_[i] = '\x00';
    }

    bool backspace() {
        bool res = false;
        if (prev()) {
            del();
            res = true;
        }
        return res;
    }

    bool next() {
        bool res = false;
        if (write_ < capacity_ - 1) {
            write_++;
            res = true;
        }
        return res;
    }

    bool prev() {
        bool res = false;
        if (write_ > 0) {
            write_--;
            res = true;
        }
        return res;
    }

    size_t home() {
        size_t res = write_;
        write_ = 0;
        return res;
    }

    size_t end() {
        size_t n;
        for (n = 0; n < capacity_ - 1; ++n)
            if (pool_[n] == '\x00') break;
        return n;
    }
};

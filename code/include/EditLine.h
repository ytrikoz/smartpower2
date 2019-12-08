#pragma once

#include <Arduino.h>

class EditLine : Print {
   public:
    EditLine() : EditLine(INPUT_MAX_LENGTH) {}

    EditLine(size_t size) : capacity_(size < 2 ? 2 : size), write_(0), read_(0) {
        pool_ = new char[size];
        memset(pool_, 0, capacity_);
    }

    EditLine(const EditLine &src) {
        capacity_ = src.capacity_;
        write_ = src.write_;
        memcpy(pool_, src.pool_, src.write_);
    }

    EditLine(const char *str) : EditLine(strlen(str) + 1) {
        write((const uint8_t *)str, strlen(str));
    }

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

    ~EditLine() {
        delete[] pool_;
    }

    void clear() {
        memset(pool_, 0, capacity_);
        write_ = 0;
        read_ = 0;
    }

    void resize(size_t size) {
        char *tmp = pool_;
        pool_ = new char[size];
        memcpy(pool_, tmp, size);
        delete[] tmp;

        capacity_ = size;
        write_ = write_ > size ? size : write_;
        read_ = read_ > size ? size : read_;
    }

    size_t size() const { return capacity_; }

    size_t free() const { return capacity_ - write_; }

    size_t available() const { return write_; }

    const char *c_str() const { return pool_; }

    size_t write(char ch) {
        return write((uint8_t)ch);
    };

    size_t write(const uint8_t ch) {
        size_t n = 0;
        if (write_ < (capacity_ - write_ - 1)) {
            pool_[write_++] = ch;
            n = sizeof(ch);
        }
        return n;
    }

    size_t write(const uint8_t *ptr, const size_t size) {
        size_t n = 0;
        while (n < size) {
            uint8_t ch = ptr[n++];
            if (!write(ch))
                break;
        }
        if (pool_[write_] != '\x00')
            pool_[++write_] = '\x00';
        return n;
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

   protected:
    char *pool_;
    size_t capacity_;
    size_t write_;
    size_t read_;
};

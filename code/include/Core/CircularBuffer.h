#pragma once

#include <stddef.h>

template <typename T, size_t BUFFER_SIZE>
class CircularBuffer {
   public:
    CircularBuffer() : head_(0), tail_(0), full_(false) {}

    ~CircularBuffer() {}

    void reset() {
        head_ = 0;
        tail_ = 0;
        full_ = false;
    }

    bool empty() const {
        return (head_ == tail_) && !full_;
    }

    bool full() const {
        return full_;
    }

    size_t size() const {
        size_t res = 0;
        if (!full_) {
            if (head_ < tail_)
                res = BUFFER_SIZE + head_ - tail_;
            else
                res = head_ - tail_;
        } else {
            res = BUFFER_SIZE;
        }
        return res;
    }

    void push(const T &item) {
        if (full_) {
            tail_++;
            if (tail_ == BUFFER_SIZE)
                tail_ = 0;
        }
        pool_[head_++] = item;
        if (head_ == BUFFER_SIZE)
            head_ = 0;
        if (head_ == tail_)
            full_ = true;
    }

    bool pop(T &item) {
        bool res = false;
        if (!empty()) {
            item = pool_[tail_++];
            if (tail_ == BUFFER_SIZE) tail_ = 0;
            full_ = false;
            res = true;
        }
        return res;
    }

    bool peek(T &item) const {
        bool res = false;
        if (!empty()) {
            item = pool_[tail_];
            res = true;
        }
        return res;
    }

   private:
    T pool_[BUFFER_SIZE];
    size_t head_;
    size_t tail_;
    bool full_;
};
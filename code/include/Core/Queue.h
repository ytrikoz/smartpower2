#pragma once

#include <Arduino.h>
#include "Strings.h"

template <typename T>
class Queue : public Printable {
    enum QueueState { WRITE,
                      READ };

   public:
    Queue(){};

    Queue(size_t capacity) {
        capacity_ = capacity;
    }

   public:
    size_t printTo(Print& p) const {
        size_t n = 0;
        for (auto it = items_.cbegin(); it != items_.cend(); it++) n += p.println(*it);
        return n;
    }

    void clear() {
        items_.clear();
    }

    bool push(const T& item) {
        if (!free()) return false;
        items_.push_back(item);
        return true;
    }

    bool pop(T& item) {
        prepare(QueueState::READ);        
        item = T(items_.back());
        items_.pop_back();
        return true;
    }

    size_t available() {
        return items_.size();
    }

    bool empty() {
        return items_.size() == 0;
    }

    size_t free() {
        return capacity_ ? capacity_ - items_.size() : items_.max_size();
    }

   protected:
    size_t capacity_;
    std::vector<T> items_;
    QueueState state_;

private:
    void prepare(QueueState state) {
        if (state_ == state) return;
        switch (state) {
            case QueueState::READ:
                std::reverse(items_.begin(), items_.end());
                break;
            case QueueState::WRITE:
                break;
        }
        state_ = state;
    }
};
#pragma once

#include <Arduino.h>
#include "Strings.h"

template <typename T>
class Container : public Printable {
    enum ContainerState { WRITE,
                          READ };
   public:
    Container(){};

    Container(size_t capacity) {
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

    bool put(const T& item) {
        if (!free()) return false;
        items_.push_back(item);
        return true;
    }

    const T get() {
        prepare(ContainerState::READ);
        if (!available()) return false;
        T item = items_.back();
        items_.pop_back();
        return item;
    }

    bool get(T& item) {
        prepare(ContainerState::READ);
        if (!available()) return false;
        item = items_.back();
        items_.pop_back();
        return true;
    }

    bool available() {
        return !empty();
    }

    bool empty() {
        return items_.size() == 0;
    }

    size_t free() {
        return capacity_ ? capacity_ - items_.size() : items_.max_size();
    }

    size_t size() {
        return items_.size();
    }

   protected:
    size_t capacity_;
    std::vector<T> items_;
    ContainerState state_;

    void prepare(ContainerState state) {
        if (state_ == state) return;
        switch (state) {
            case ContainerState::READ:
                std::reverse(items_.begin(), items_.end());
                break;
            case ContainerState::WRITE:
                break;
        }
        state_ = state;
    }


};
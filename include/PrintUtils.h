#pragma once

#include "Arduino.h"

namespace PrintUtils {

size_t inline println(Print* p) {
	return p->println();
}

template <typename T>
size_t print(Print *p, T v) {
    size_t n = p->print(v);
    return n += p->print(' ');
}

template <typename T, typename... Args>
size_t print(Print *p, T first, Args... args) {
    size_t n = print(p, first);
    n += print(p, args...);
    return n;
}

template <typename T, typename... Args>
size_t println(Print *p, T first, Args... args) {
    size_t n = print(p, first, args...);
	n += println(p);
    return n;
}

}  // namespace PrintUtils
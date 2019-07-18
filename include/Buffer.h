#pragma once

#include <string.h>

class Buffer {
   private:
    char* buffer;
    int pos;
    int len;
    int capacity;

   public:
    Buffer(int capacity);
    ~Buffer();
    bool empty();
    int length();
    void clear();
    void insert(const char chr);
    void set(const char* str);
    void set(Buffer* buf);
    char* get();
    void next();
    void prev();
    void first();
    void last();
    void del();
    void del_next();
};
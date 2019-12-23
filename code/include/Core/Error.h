#ifndef ERROR_H
#define ERROR_H

#pragma once

#include <Arduino.h>

enum ErrorCode {
    ERROR_INIT = -1,
    ERROR_START = -2,    
    ERROR_PARAM = -100,
    ERROR_NETWORK = -200,
    ERROR_EXECUTE = -300,
    ERROR_PARSE = -400
};

class Error : public Printable {
   public:
    static Error ok() { return Error(); }

   public:
    Error() {
        code_ = 0;
        memset(description_, 0, OUTPUT_MAX_LENGTH);
    };

    Error(const ErrorCode code) {
        code_ = code;
    }

    Error(const ErrorCode code, const __FlashStringHelper *pstr) : Error(code) {
        strcpy_P(description_, (PGM_P)pstr);
    }

    Error(const ErrorCode code, const char *description) : Error(code) {
        strncpy(description_, description, OUTPUT_MAX_LENGTH);
    }

    int code() const { return code_; }

    const char *description() const { return description_; }

    operator bool() const { return code_ != 0; }
    
    virtual size_t printTo(Print& p) const {
        size_t n = 0;
        n += p.print(code_);
        n += p.print(':');
        n += p.print(description_);
        return n;
    }

   private:
    int code_;
    char description_[OUTPUT_MAX_LENGTH];
};

#endif
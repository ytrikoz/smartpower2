#ifndef ERROR_H
#define ERROR_H

#pragma once

#include <Arduino.h>

enum ErrorCode {
    ERROR_INIT = -1,
    ERROR_START = -2,
    WRONG_PARAM = -3,
    ERROR_UNSUPPORTED = -4,
    ERROR_EXECUTE = -5,
    ERROR_SIZE = -6,
    ERROR_NETWORK = -200,    
    ERROR_JSON = -400,
};

class Error : public Printable {
   public:
    static Error none() { return Error(); }

   public:
    Error() {
        code_ = 0;
        memset(desc_, 0, OUTPUT_MAX_LENGTH);
    }

    Error(const ErrorCode code) : Error() {
        code_ = code;
    }

    Error(const ErrorCode code, const __FlashStringHelper *pstr) : Error(code) {
        strcpy_P(desc_, (PGM_P)pstr);
    }

    Error(const ErrorCode code, const char *desc) : Error(code) {
        strncpy(desc_, desc, OUTPUT_MAX_LENGTH);
    }

    int code() const { return code_; }

    const char *description() const { return desc_; }

    operator bool() const { return code_ != 0; }

    virtual size_t printTo(Print &p) const {
        size_t n = 0;
        n += p.print(code_);
        n += p.print(':');
        n += p.print(' ');
        n += p.print(desc_);
        return n;
    }

    virtual String toString() const {
        String str;
        str += code_;
        str += ':';
        str += ' ';
        str += desc_;
        return str;
    }

   private:
    int code_;
    char desc_[OUTPUT_MAX_LENGTH];
};

#endif
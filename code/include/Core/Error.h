#ifndef ERROR_H
#define ERROR_H

#pragma once

#include <Arduino.h>

enum ErrorCode {
    ERROR_INIT = -1,
    ERROR_START = -2,    
    WRONG_PARAM = -3,
    ERROR_UNSUPPORTED = -4,
    ERROR_NETWORK = -200,
    ERROR_EXECUTE = -300,
    ERROR_JSON = -400, 
    ERROR_SIZE = -500
};

class Error : public Printable {
   public:
    static Error ok() { return Error(); }
    static Error BufferLow(size_t size) {
        return Error(ERROR_SIZE, String(size).c_str());
    }
    static Error WrongParameter(const __FlashStringHelper* name) {
        return Error(WRONG_PARAM, name);
    }
    static Error NetworkRelated(const String message) {
        return Error(ERROR_NETWORK, message.c_str());
    }
   public:
    Error() {
        code_ = 0;
        memset(desc_, 0, OUTPUT_MAX_LENGTH);
        strcpy(desc_, "ok");
    }

    Error(const ErrorCode code) {
        code_ = code;
        strcpy_P(desc_, str_unsupported);
    }

    Error(const ErrorCode code, const __FlashStringHelper *pstr): Error() {
        code_ = code;
        strcpy_P(desc_, (PGM_P) pstr);
    }

    Error(const ErrorCode code, const char *desc) : Error() {
        code_ = code;
        strncpy(desc_, desc, OUTPUT_MAX_LENGTH);
    }

    int code() const { return code_; }

    const char *description() const { return desc_; }

    operator bool() const { return code_ != 0; }
    
    virtual size_t printTo(Print& p) const {
        size_t n = 0;
        n += p.print(code_);
        n += p.print(':');
        n += p.print(' ');
        n += p.print(desc_);
        return n;
    }

   private:
    int code_;
    char desc_[OUTPUT_MAX_LENGTH];
};

#endif
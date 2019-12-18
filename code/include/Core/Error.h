#ifndef ERROR_H
#define ERROR_H

#pragma once

#include <Arduino.h>

enum ErrorCode {
    ERROR_INIT = -1,
    ERROR_START = -2,
    ERROR_PARAM = -100,
    ERROR_NETWORK = -200,
    ERROR_EXECUTE = -300
};

class Error {
   public:
    static Error ok() { return Error(); }

   public:
   
    Error(const ErrorCode code, const __FlashStringHelper *pstr) {
        char buf[64]; 
        strcpy_P(buf, (PGM_P) pstr);
        set(code, buf);
    } 

    Error(const ErrorCode code, const char *desc = "") {
        set(code, "none");
    }

    Error() {
        code_ = 0;
        memset(message_, 0, sizeof(message_));
    };

    void set(const ErrorCode code, const char* desc) {
        switch (code) {
            case ERROR_PARAM:
                sprintf(message_, "param not set: %s", desc);
                break;
            case ERROR_NETWORK:
                sprintf(message_, "network related: %s", desc);
                break;
            case ERROR_EXECUTE: 
                sprintf(message_, "on execute: %s", desc);
                break;
            default:
                sprintf(message_,  "unknown: %s", desc);
                break;
        }
    }
    
    int code() const { return code_; }

    const char *message() const { return message_; }

    operator bool() const { return code_ == 0; }

   private:
    int code_;
    char message_[128];
};

#endif
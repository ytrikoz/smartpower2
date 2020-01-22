#ifndef ERROR_H
#define ERROR_H

#pragma once

#include <Arduino.h>

enum ErrorSeverity {
    ERROR_IGNORE,
    ERROR_WARNING,
    ERROR_ALERT,
    ERROR_CRITICAL
};

enum ErrorCode {
    ERROR_INIT = -1,
    ERROR_START = -2,
    INVALID_PARAM = -3,
    UNKNOWN_OBJECT = -4,
    ERROR_SERIALIZE = -5,
    ERROR_SIZE = -6,
    ERROR_VOLTAGE_LOW = -100,
    ERROR_NO_LOAD = -101,
    ERROR_NETWORK = -200,
};

class Error : public Printable {
   public:
    static Error none() { return Error(); }

   public:
    Error() {
        code_ = 0;
        severity_ = ERROR_IGNORE;
        memset(desc_, 0, OUTPUT_MAX_LENGTH);
        memset(source_, 0, OUTPUT_MAX_LENGTH);
    }

    Error(const ErrorCode code) : Error() {
        code_ = code;
        severity_ = ERROR_CRITICAL;
    }

    Error(const ErrorCode code, ErrorSeverity severity) : Error(code) {
        severity_ = severity;
    }

    Error(const ErrorCode code, const __FlashStringHelper *pstr) : Error(code) {
        strcpy_P(desc_, (PGM_P) pstr);
    }

    Error(const ErrorCode code, const char *desc) : Error(code) {
        strncpy(desc_, desc, OUTPUT_MAX_LENGTH);
    }

    int code() const { return code_; }

    const char *description() const { return desc_; }

    operator bool() const { return code_ != 0; }

    virtual String source() const {
        return source_;
    }

    virtual ErrorSeverity severity() {
        return severity_;
    }

    virtual String toString() const {
        String str;
        str += code_;
        str += ':';
        str += ' ';
        str += getErrorStr();
        if (strlen(desc_)) {
            str += '-';
            str += desc_;
        }
        return str;
    }

    virtual size_t printTo(Print &p) const {
        size_t n = 0;
        n += p.print(code_);
        n += p.print(':');
        n += p.print(' ');
        n += p.print(getErrorStr());
        if (strlen(desc_)) {
            n += p.print('-');
            n += p.print(desc_);
        }
        return n;
    }

    virtual String getErrorStr() const {
        String str;
        switch (code_) {
            case ERROR_INIT:
                str = F("on init");
                break;
            case ERROR_START:
                str = F("on start");
                break;
            case INVALID_PARAM:
                str = F("invalid param");
                break;
            case UNKNOWN_OBJECT:
                str = F("unhandled data");
                break;
            case ERROR_SERIALIZE:
                str = F("serialize");
                break;
            case ERROR_SIZE:
                str = F("size too large");
                break;
            case ERROR_VOLTAGE_LOW: 
                str = F("voltage low");
                break;
            case ERROR_NO_LOAD:
                str = F("load low");
                break;
            case ERROR_NETWORK:
                str = F("network related");
                break;
            default:
                str = F("unknown");
                break;
        }
        return str;
    }

   private:
    ErrorSeverity severity_;
    int code_;
    char desc_[OUTPUT_MAX_LENGTH];
    char source_[OUTPUT_MAX_LENGTH];
};

#endif
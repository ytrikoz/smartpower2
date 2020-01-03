#pragma once

#include "Strings.h"
#include "BuildConfig.h"
#include "Core/CircularBuffer.h"

#define SYSLOG_FACILITY 1

enum LogSeverity { LOG_ALERT = 1,
                   LOG_INFO = 6,
                   LOG_DEBUG = 7 };

class StringPusher {
    public:
        virtual bool push(const String&) = 0;
};

class StringPullable {
    public:
        virtual bool pull(String&) = 0;
};

class Logger {
   public:
    virtual void alert(const String& message) { log(LOG_ALERT, message.c_str()); }

    virtual void info(const String& message) { log(LOG_INFO, message.c_str()); }

    virtual void alert(const String& module, const String& message) { log(LOG_ALERT, module.c_str(), message.c_str()); }

    virtual void info(const String& module, const String& message) { log(LOG_INFO, module.c_str(), message.c_str()); }
    
    virtual bool log(LogSeverity level, const char* message) {    
        return log(level, "", message);
    }
    
    virtual bool log(const LogSeverity level, const __FlashStringHelper* module, const  __FlashStringHelper* message) {
        return log(level, String(module).c_str(), String(message).c_str());
    }

    virtual bool log(LogSeverity level, const char* module, const char* message) = 0;
};

class TextLog : public Logger {
   public:
    virtual bool log(const LogSeverity level, const char* routine, const char* message) override {
        char buf[16];
        String str = getLogLevelStr(buf, level);
        str += " [";
        str += routine;
        str += "] ";
        str += message;
        return push(str);
    };

   protected:
    virtual bool push(const String& str) = 0;

   private:
    char* getLogLevelStr(char* buf, LogSeverity level) {
        PGM_P strP;
        switch (level) {
            case LOG_ALERT:
                strP = str_error;
                break;
            case LOG_INFO:
                strP = str_warn;
                break;
            case LOG_DEBUG:
                strP = str_info;
                break;
            default:
                strP = str_unknown;
                break;
        }
        char str[16];
        strncpy_P(str, strP, 15);
        for (char* p = str; *p; p++) *p = toupper(*p);
        return buf;
    }
};

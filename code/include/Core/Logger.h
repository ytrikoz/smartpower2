#pragma once

#include "Strings.h"
#include "Core/CircularBuffer.h"

enum LogSeverity { LOG_ALERT = 1,
                   LOG_INFO = 6,
                   LOG_DEBUG = 7 };
class AbstactLog {
   public:
    virtual void log(LogSeverity level, const String& str) {
        String tmp;
        log(level, tmp, str);
    }

    virtual void log(const LogSeverity level, const String& routine, const String& message) = 0;

    virtual void alert(const String& str) { log(LOG_ALERT, str); }

    virtual void info(const String& str) { log(LOG_INFO, str); }
};

class TextLog : public AbstactLog {
   public:
    virtual size_t push(const String& str) = 0;

    virtual void log(const LogSeverity level, const String& routine, const String& message) override {
        char buf[16];
        String str = getLogLevelStr(buf, level);
        str += " [";
        str += routine;
        str += "] ";
        str += message;
        push(str);
    };

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
        strncpy_P(str, strP, 13);
        for (char* p = str; *p; p++)
            *p = toupper(*p);
        sprintf(buf, "[%s]", str);
        return buf;
    }
};


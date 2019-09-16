#pragma once

#include <Arduino.h>
#include <CommonTypes.h>
#include "Config.h"

template <typename T>

T pgmtoc(T pgmstr) {
    char buf[128];
    strcpy_P(buf, (char*)pgm_read_ptr(&pgmstr));
    return buf;
}

template <Print*, typename... Args>
void say(Print* p, Args... args) {
    for (size_t n = 0; n < sizeof...(args); ++n) {
        char buf[128];
        p->print(args...);
    }
}

template <Print*, AppModuleEnum, const char*, typename... Args>
void say(Print* p, AppModuleEnum module, const char* fmt, Args... args) {
    char buf[32];
    buf[0] = '[';
    strcat_P(&buf[1], module_strP[module]);
    size_t n = strlen(buf);
    buf[n] = ']';
    buf[++n] = '\x00';

    foo(bar(args)...);
}

class AppModule {
   public:
    AppModule(AppModuleEnum module) {
        char buf[32] = "[";
        PGM_P pgmStr = module_strP[module];
        strcpy_P(&buf[1], (char*)pgm_read_ptr(&pgmStr));
        size_t n = strlen(buf);
        buf[n] = ']';
        buf[++n] = '\x00';
        moduleName = String(buf);
    }
    AppModuleEnum getModule() { return module; }
    
    Print* getOutput() { return this->out; }
    
    void setOutput(Print* p) { this->out = p; }

    String getModuleName() { return moduleName; }

   public:
    virtual void setConfig(Config* config){};

   protected:
    size_t say_P(PGM_P pgmStr) { return out->print(pgmStr); };

    size_t say(char* str) { return out->print(str); };

    size_t sayf_P(const char* fmt, ...) {
        char buf[128];
        size_t n = out->print(getModuleName());
        n += out->print(' ');
        va_list args;
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        n += out->print(buf);
        return n;
    }

    size_t sayf(const char* fmt, ...) {
        char buf[128];
        size_t n = out->print(getModuleName());
        n += out->print(' ');
        va_list args;
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        n += out->print(buf);
        return n;
    }
    Print* out = &USE_SERIAL;

   private:
    AppModuleEnum module;
    String moduleName;
};
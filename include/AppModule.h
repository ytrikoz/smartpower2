#pragma once

#include <Arduino.h>
#include <CommonTypes.h>
#include "Config.h"

static PGM_P module_strP[] PROGMEM = {
    str_btn,  str_clock,  str_led, str_psu,    str_task,  str_shell, str_lcd,
    str_http, str_netsvc, str_ntp, str_telnet, str_shell, str_update};

static String getModuleName(AppModuleEnum module) {
    PGM_P pgmStr = (char*)pgm_read_ptr(&(module_strP[module]));
    return StrUtils::getStrP(pgmStr);
}

template <Print*, typename... Args>
void sayArgsP(Print* p, Args... args) {
    char buf[128]; 
    for (size_t n = 0; n < sizeof...(args); ++n) {        
        memset(buf, 0, 128);
        strcpy(buf, (char*)pgm_read_ptr(&args[n]...));
        p->print(buf);
    }
}

template <Print*, AppModuleEnum, const char*, typename... Args>
void say(Print* p, AppModuleEnum module, const char* fmt, Args... args) {
    char buf[32];
    buf[0] = '[';    
    strcpy_P(buf[1], getModuleName(module).c_str());
    size_t n = strlen(buf);
    buf[n] = ']';
    buf[++n] = '\x00';

    sayArgsP(args...);
}

class AppModule {
   public:
    AppModule(AppModuleEnum module) {        
        moduleName = getModuleName(module);
    }
    AppModuleEnum getModule() { return module; }
    
    Print* getOutput() { return this->out; }
    
    void setOutput(Print* p) { this->out = p; }

   public:
    virtual void setConfig(Config* config){};

   protected:
    size_t say_P(PGM_P pgmStr) { return out->print(pgmStr); };
 
    size_t say(char* str) { return out->print(str); };

    size_t sayf_P(const char* fmt, ...) {
        va_list args;
        return sayf(fmt, args);
    }

    size_t sayf(const char* fmt, ...) {
        char buf[128];
        va_list args;
        va_start(args, fmt);
        vsprintf(buf, fmt, args);
        va_end(args);
        return out->print(buf);
    }
    Print* out = &USE_SERIAL;

   private:

    AppModuleEnum module;
    String moduleName;
};
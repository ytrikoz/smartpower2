#pragma once

#include <Arduino.h>
#include <CommonTypes.h>
#include "Config.h"

// template <Print*, typename... Args>
// void sayArgsP(Print* p, Args... args) {
//     char buf[128];
//     for (size_t n = 0; n < sizeof...(args); ++n) {
//         memset(buf, 0, 128);
//         strcpy(buf, (char*)pgm_read_ptr(&args[n]...));
//         p->print(buf);
//     }
// }

// template <Print*, AppModuleEnum, const char*, typename... Args>
// void say(Print* p, AppModuleEnum module, const char* fmt, Args... args) {
//     String str = getModuleName(module);
//     str =  StrUtils::getIdentStr(str);
//     p->print(str);
//     sayArgs(args...);
// }
enum AppModuleEnum {
    MOD_BTN,
    MOD_CLOCK,
    MOD_LED,
    MOD_PSU,
    MOD_PSU_LOG,
    MOD_TASK,
    MOD_TELNET_SHELL,
    MOD_LCD,
    MOD_HTTP,
    MOD_NETSVC,
    MOD_NTP,
    MOD_TELNET,
    MOD_SERIAL_SHELL,
    MOD_UPDATE
};

extern String getModuleName(size_t index);
extern String getModuleName(AppModuleEnum module);

class AppModule {
   public:
    AppModule(AppModuleEnum module);
    void setOutput(Print* p) { this->out = p; }
    Print* getOutput() { return this->out; }

   public:
    virtual void setConfig(Config* config){ this->config = config;};
    virtual bool begin() { return false; };
    virtual void end(){};
    void printDiag() { printDiag(out); };

   protected:
    virtual void printDiag(Print* p) { p->println(); };
    size_t saylnf(const char* fmt, ...);
    size_t saylnf_P(const char* fmt, ...);
    size_t sayln_P(PGM_P pgmStr);
    Print* err = &USE_SERIAL;
    Print* out = &USE_SERIAL;
    Config* config;
   private:
    size_t say(char* str);
    size_t say(String& str);
    

   private:
    AppModuleEnum module;
    String moduleName;
};

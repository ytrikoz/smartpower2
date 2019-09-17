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
extern String getModuleName(size_t index);
extern String getModuleName(AppModuleEnum module);

class AppModule {
   public:
    AppModule(AppModuleEnum module);
    void setOutput(Print* p) { this->out = p; }
    Print* getOutput() { return this->out; }

   public:
    virtual void setConfig(Config* config){};
    virtual bool begin() { return true; };
    virtual void end(){};
    virtual void printDiag() { };

   protected:
    size_t sayf(const char* fmt, ...);
    size_t say_P(PGM_P pgmStr);

   private:
    size_t say(char* str);
    size_t say(String& str);
    size_t sayf_P(const char* fmt, ...);

   private:
    AppModuleEnum module;
    String moduleName;
    Print* out = &USE_SERIAL;
};

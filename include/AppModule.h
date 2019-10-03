#pragma once

#include <Arduino.h>

#include "AppUtils.h"
#include "CommonTypes.h"
#include "Config.h"
#include "PrintUtils.h"

class Named {
  public:
    Named(AppModuleEnum module);
    void setOutput(Print *p);

    const char *getName();

  protected:
    size_t say(char *str);
    size_t say_strP(PGM_P strP);
    size_t say_strP(PGM_P strP, char *value);
    size_t say_strP(PGM_P strP, int value);

  protected:
    Print *out = &USE_SERIAL;
    Print *dbg = &USE_SERIAL;
    Print *err = &USE_SERIAL;

  private:
    size_t print_name(Print *);
    size_t print(Print *, char);
    size_t print(Print *, const char *);
    size_t print(Print *, char *, size_t);

  private:
    AppModuleEnum module;
    char *name;
};

class AppModule : public Named {
  public:
    AppModule(AppModuleEnum module);
    virtual void init(Config *config) final;
    virtual bool begin() { return false; };
    virtual void end(){};
    virtual void start(){};
    virtual void stop(){};
    virtual void loop(){};
    virtual size_t printDiag(Print *p);
    size_t printDiag() { return printDiag(out); };

  protected:
    virtual void setConfig(Config *){};
    Config *config;
};

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

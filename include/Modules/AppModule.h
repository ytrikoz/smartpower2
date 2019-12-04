#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "AppUtils.h"
#include "Config.h"
#include "PrintUtils.h"
#include "Wireless.h"

using namespace AppUtils;
using namespace PrintUtils;
using namespace StrUtils;

enum ModState {
    STATE_INIT,
    STATE_INIT_FAILED,
    STATE_INIT_COMPLETE,
    STATE_START_FAILED,
    STATE_ACTIVE
};

namespace {
const String getModStateStr(ModState state) {
    String str;
    switch (state) {
        case STATE_INIT:
            str = F("Not initialized");
            break;
        case STATE_INIT_FAILED:
            str = F("Initialize failed");
            break;
        case STATE_INIT_COMPLETE:
            str = F("Intialized");
            break;
        case STATE_START_FAILED:
            str = F("Start failed");
            break;
        case STATE_ACTIVE:
            str = F("Active");
            break;
        default:
            str = FPSTR(str_unknown);
    }
    return str;
}
}  // namespace

class AppModule {
   public:
    AppModule(AppModuleEnum mod) {
        module_ = mod;
        state_ = STATE_INIT;
    };

    void setOutput(Print *p) { this->out = this->dbg = this->err = p; }

    const String getName() {
        char buf[16];
        strcpy_P(buf, (char *)pgm_read_ptr(&(mod_name[module_])));
        return buf;
    }

    void setConfig(Config *config) { config_ = config; }

    bool init() {
        if (state_ != STATE_INIT)
            return false;

        state_ = onInit() ? STATE_INIT_COMPLETE : STATE_INIT_FAILED;

        if (state_ != STATE_INIT_COMPLETE) {
            print_ident(dbg, getName());
            println(dbg, FPSTR(str_init), FPSTR(str_failed));
        }

        return state_ == STATE_INIT_COMPLETE;
    }

    bool start() {
        if (state_ == STATE_INIT_FAILED)
            return false;
        if (state_ == STATE_INIT_COMPLETE || init()) {
            state_ = onStart() ? STATE_ACTIVE : STATE_START_FAILED;
            if (state_ != STATE_ACTIVE) {
                print_ident(dbg, getName());
                println(dbg, FPSTR(str_load), FPSTR(str_failed));
            }
        };
        return state_ == STATE_ACTIVE;
    }

    virtual void stop(){};

    void end() {
        if (state_ != STATE_ACTIVE)
            return;
        onDeinit();
        state_ = STATE_INIT;
    };

    void loop() {
        if (state_ < STATE_START_FAILED)
            return;

        if (STATE_ACTIVE || start()) onLoop();
    };

    size_t printDiag() {
        return printDiag(out);
    };

    size_t printDiag(Print *p) {
        size_t n = println_nameP_value(out, str_state, getModStateStr(state_));
        return n += onDiag(out);
    }

    virtual size_t onDiag(Print *p) {
        return 0;
    }

    virtual bool isCompatible(Wireless::NetworkMode value) { return true; };

    virtual bool isNetworkDepended() { return false; }

    size_t say(const char *str) {
        size_t n = print_ident(out, getName());
        n += print(out, str);
        return n += print_ln(out);
    }

    size_t say_strP(PGM_P strP) {
        size_t n = print_ident(out, getName());
        char buf[64];
        strcpy_P(buf, strP);
        n += print(out, buf);
        return n += print_ln(out);
    }

    size_t say_strP(PGM_P strP, int value) {
        String str = String(value, DEC);
        return say_strP(strP, str.c_str());
    }

    size_t say_strP(PGM_P strP, const char *value) {
        size_t n = print_ident(out, getName());
        n += println_nameP_value(out, strP, value);
        return n;
    }

   protected:
    virtual bool onInit() { return true; };
    virtual void onDeinit(){};
    virtual bool onStart() { return true; }
    virtual void onStop(){};
    virtual void onLoop() = 0;

   protected:
    Print *out = &INFO;
    Print *dbg = &INFO;
    Print *err = &INFO;
    ModState state_;
    Config *config_;

   private:
    char name_[16];
    AppModuleEnum module_;
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

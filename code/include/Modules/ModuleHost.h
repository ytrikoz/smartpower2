#pragma once

#include "Module.h"

struct ModuleDefine {
    PGM_P name;
    Module* obj;
    bool AP_mode;
    bool STA_mode;
};

class ModuleHost {
    public:
    void setOutput(Print* p) {
        out_ = p;
    }

    String name(uint8_t index) const {
         return name(ModuleEnum(index));
    }

    Module *module(uint8_t index) {
        return instance(ModuleEnum(index));
    }

    Module *module(const String& name) {
        ModuleEnum module;
        if (get(name.c_str(), module))
            return instance(module);
        else 
            return 0;
    }

    bool init(ModuleEnum item) {
        auto obj = instance(item);
        bool res = obj && obj->init();        
        if (!res) {
            PrintUtils::print_ident(out_, name(item).c_str());
            PrintUtils::println(out_, FPSTR(str_init), FPSTR(str_failed));
        }
        return res;
    }

    void initModules() {
        for (size_t i = 0; i < APP_MODULES; ++i) {
            init(ModuleEnum(i));
        }     
    }

    virtual bool start(ModuleEnum item) {
        auto obj = instance(item);
        bool res = obj && obj->start();
        if (!res) {
            PrintUtils::print_ident(out_, name(item));
            PrintUtils::println(out_, FPSTR(str_start), FPSTR(str_failed));
        }
        return res;
    };
    
    virtual void stop(ModuleEnum mod) {
        auto obj = instance(mod);
        if (obj) obj->stop();
    };

    virtual Module *instance(const ModuleEnum) = 0;

    virtual String name(const ModuleEnum) const = 0;

    virtual bool get(const char *, ModuleEnum&) const = 0;

protected:
    Print *out_ = &Serial;
};


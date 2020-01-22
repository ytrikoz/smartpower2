#pragma once

#include "Core/Module.h"
#include "MainLog.h"

struct ModuleDef {
    Module *obj;
    PGM_P name;
    bool network;
};

class Host {
   public:
    virtual String getName(uint8_t index) const = 0;

    virtual Module *getInstance(uint8_t index) const = 0;

    bool init(uint8_t index) {
        auto obj = getInstance(index);
        bool res = obj->init();
        if (!res) {
            err_ = Error(ERROR_INIT, getName(index).c_str());
        }
        return res;
    }

    bool start(uint8_t index) {
        auto obj = getInstance(index);
        bool res = obj->start();
        if (!res) {
            err_ = Error(ERROR_START, getName(index).c_str());
        }
        return res;
    };

    void stop(uint8_t index) {
        auto obj = getInstance(index);
        if (obj) obj->stop();
    };
    
    void setOutput(MainLog *p) {
        out_ = p;
    }

   protected:
    Error err_;
    MainLog *out_;
};

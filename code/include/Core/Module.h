#pragma once

#include "CommonTypes.h"

#include "Config.h"
#include "Wireless.h"

#include "Utils/PrintUtils.h"
#include "Utils/StrUtils.h"
#include "Utils/FsUtils.h"

class Module {
   public:
    Module() : modState_(ModuleState::STATE_INIT) {}

    void setOutput(Print *p) { out_ = p; }

    void setConfig(Config *c) { config_ = c; }
    
    bool changeConfig(ConfigItem param, const char* value) {
        return onConfigChange(param, value);
    }

    bool execute(const String &param, const String &value) {
        Error res = onExecute(param, value);
        if (!res) {
            PrintUtils::print(out_, res.code(), res.description());
            PrintUtils::println(out_);
        }
        return res;
    }

    bool init() {
        if (modState_ > ModuleState::STATE_INIT_COMPLETE) return true;
        if (modState_ == ModuleState::STATE_INIT_FAILED) return false;
        
        modState_ = onInit() ? STATE_INIT_COMPLETE : ModuleState::STATE_INIT_FAILED;                
        return modState_ == ModuleState::STATE_INIT_COMPLETE;
    }

    bool start() {        
        if (modState_ == ModuleState::STATE_ACTIVE) return true;
        if (modState_ == ModuleState::STATE_START_FAILED) return false;

        if (modState_ < ModuleState::STATE_INIT_COMPLETE)  {
            if (!init()) return false;           
        }        
        modState_ = onStart() ? STATE_ACTIVE: ModuleState::STATE_START_FAILED;        
        return modState_ == ModuleState::STATE_ACTIVE;
    }

    void stop(){
        if (modState_ < ModuleState::STATE_ACTIVE) return;
        onStop();
        modState_ = ModuleState::STATE_INIT_COMPLETE;
    };

    void end() {
        if (modState_ < ModuleState::STATE_INIT_FAILED) return;
        onDeinit();
        modState_ = ModuleState::STATE_INIT;
    };

    void loop() {   
        if (modState_ == ModuleState::STATE_ACTIVE || start()) onLoop();
    };

    size_t printDiag(void) {
        return printDiag(out_);
    };

    size_t printDiag(Print *p) {
        StaticJsonDocument<256> doc;
        doc[FPSTR(str_state)] = getModuleStateStr();
        JsonVariant obj = doc.as<JsonObject>();
        onDiag(obj);
        return serializeJsonPretty(doc, *p);
    };

    String getModuleStateStr() {
        return getModuleStateStr(modState_);
    }

    virtual void onDiag(const JsonObject &obj) {}

   protected:
    virtual bool onInit() { return true; };

    virtual void onDeinit(){};

    virtual bool onStart() { return true; }

    virtual void onStop(){};

    virtual void onLoop() = 0;

    virtual bool onConfigChange(const ConfigItem item, const char* value) {
        return true;
    }

    virtual Error onExecute(const String &param, const String &value) {
        Error err = Error(ERROR_EXECUTE, FPSTR(str_unsupported));
        return err;
    }

    String getModuleStateStr(const ModuleState state) {
        PGM_P strP;
        switch (state) {
            case STATE_INIT:
                strP = str_wait;
                break;
            case STATE_INIT_FAILED:
                strP = str_invalid;
                break;
            case STATE_INIT_COMPLETE:
                strP = str_ready;
                break;
            case STATE_START_FAILED:
                strP = str_failed;
                break;
            case STATE_ACTIVE:
                strP = str_active;
                break;
            default:
                strP = str_unknown;
        }
        return String(FPSTR(strP));
    }
protected:
    void setError(Error e){
        modError_ = e;
    }
    void setError(ErrorCode code, const char* description) {
        modError_ = Error(code, description);
    }
    Print *out_;
    Config *config_;
private:
    Error modError_;
    ModuleState modState_;    
};

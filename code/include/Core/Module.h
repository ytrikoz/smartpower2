#pragma once

#include "Config.h"
#include "Wireless.h"

#include "Utils/ModuleUtils.h"
#include "Utils/PrintUtils.h"
#include "Utils/StrUtils.h"
#include "Utils/FsUtils.h"

class Module {
   public:
    Module() : modState_(ModuleState::STATE_INIT) {}

    void setOutput(Print *p) { out_ = p; }

    void setConfig(Config *c) { config_ = c; }
    
    bool changeConfig(const ConfigItem param, const String& value) {
        return onConfigChange(param, value);
    }

    bool execute(const String &param, const String &value) {
        setError(Error::ok());
        onExecute(param, value);
        return !failed();            
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
        if(failed()) {
            doc[FPSTR(str_error)] = getError().description();
        }
        JsonVariant obj = doc.as<JsonObject>();
        onDiag(obj);
        return serializeJsonPretty(doc, *p);
    };

    virtual String getModuleStateStr() {
        return ModuleUtils::getStateStr(modState_);
    }

    virtual void onDiag(const JsonObject &obj) {}
    
    virtual Error getError() {
        return modError_;
    }
   protected:
    virtual bool onInit() { return true; };

    virtual void onDeinit(){};

    virtual bool onStart() { return true; }

    virtual void onStop(){};

    virtual void onLoop() = 0;

    virtual bool onConfigChange(const ConfigItem item, const String& value) {
        return true;
    }
    
    virtual bool failed() const {
        return modError_.code() != 0;
    }

    virtual void onExecute(const String &param, const String &value) {
        setError(ERROR_UNSUPPORTED);        
    }

   
protected:    
    void setError(Error e){
        modError_ = e;
    }
    void setError(ErrorCode code) {
        modError_ = Error(code);
    }
    void setError(ErrorCode code, const char* desc) {
        modError_ = Error(code, desc);
    }
    void setError(ErrorCode code, String desc) {
        modError_ = Error(code, desc.c_str());
    }
    void setError(ErrorCode code, ConfigItem param) {
        modError_ = Error(code, config_->name(param).c_str());
    }
    Print *out_;
    Config *config_;
    
private:
    Error modError_;
    ModuleState modState_;    
};

#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>

#include "CommonTypes.h"
#include "AppUtils.h"
#include "Config.h"
#include "Wireless.h"
#include "PrintUtils.h"
#include "StrUtils.h"
#include "FsUtils.h"

enum ModuleState {
    STATE_INIT,
    STATE_INIT_FAILED,
    STATE_INIT_COMPLETE,
    STATE_START_FAILED,
    STATE_ACTIVE
};

class Module {
   public:
    Module(): state_(STATE_INIT) {}
    
    void setOutput(Print *p) { out_ = p; }

    void setConfig(Config *c) { config_ = c; }

    bool execute(const String& param, const String& value) {        
        Error res = onExecute(param, value);
        if(!res) PrintUtils::println(out_, res.code(), res.message());
        return res;
    }

    bool init() {
        if (state_ > STATE_INIT)
            return false;
        state_ = onInit() ? STATE_INIT_COMPLETE : STATE_INIT_FAILED;
        return state_ == STATE_INIT_COMPLETE;
    }

    bool start() {
        if (state_ == STATE_INIT_FAILED)
            return false;
        if (state_ == STATE_INIT_COMPLETE || init())
            state_ = onStart() ? STATE_ACTIVE : STATE_START_FAILED;
        return state_ == STATE_ACTIVE;
    }

    virtual void stop(){};

    void end() {
        if (state_ < STATE_INIT_FAILED) return;
        onDeinit();
        state_ = STATE_INIT;
    };

    void loop() {
        if (state_ == STATE_START_FAILED) 
            return;

        if (state_ == STATE_ACTIVE || start()) onLoop();
    };

    size_t printDiag() {
        return printDiag(out_);
    };

    size_t printDiag(Print *p) {
        StaticJsonDocument<256> doc;
        doc[FPSTR(str_state)] = state_;   
        JsonVariant obj = doc.as<JsonObject>();
        onDiag(obj);
        return serializeJsonPretty(doc, *p);
    };

    virtual bool isCompatible(NetworkMode value) { return true; };

    virtual bool isNetworkDepended() { return false; }

    virtual void onDiag(const JsonObject& obj) { }

   protected:
    virtual bool onInit() { return true; };
    
    virtual void onDeinit(){};
    
    virtual bool onStart() { return true; }
    
    virtual void onStop(){};
    
    virtual void onLoop() = 0;

    virtual Error onExecute(const String& paramStr, const String& valueStr) {         
        Error err = Error(ERROR_EXECUTE, FPSTR(str_unsupported));
        return err;
    }

    void setError(const String &str);

   protected:
    ModuleState state_;
    Error error_;
    Print *out_;
    Config *config_;
};

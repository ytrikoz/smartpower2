#pragma once

#include "CommonTypes.h"

#include "Config.h"
#include "Wireless.h"

#include "Utils/PrintUtils.h"
#include "Utils/StrUtils.h"
#include "Utils/FsUtils.h"

class Module {
   public:
    Module() : state_(ModuleState::STATE_INIT) {}

    void setOutput(Print *p) { out_ = p; }

    void setConfig(Config *c) { config_ = c; }

    bool execute(const String &param, const String &value) {
        Error res = onExecute(param, value);
        if (!res) {
            PrintUtils::print(out_, res.code(), res.message());
            PrintUtils::println(out_);
        }
        return res;
    }

    bool init() {
        if (state_ > ModuleState::STATE_INIT)
            return false;
        state_ = onInit() ? ModuleState::STATE_INIT_COMPLETE : ModuleState::STATE_INIT_FAILED;
        return state_ == ModuleState::STATE_INIT_COMPLETE;
    }

    bool start() {
        if (state_ == ModuleState::STATE_INIT_FAILED)
            return false;
        if (state_ == ModuleState::STATE_INIT_COMPLETE || init())
            state_ = onStart() ? ModuleState::STATE_ACTIVE : ModuleState::STATE_START_FAILED;
        return state_ == ModuleState::STATE_ACTIVE;
    }

    virtual void stop(){};

    void end() {
        if (state_ < ModuleState::STATE_INIT_FAILED) return;
        onDeinit();
        state_ = ModuleState::STATE_INIT;
    };

    void loop() {
        if (state_ == ModuleState::STATE_START_FAILED)
            return;
        if (state_ == ModuleState::STATE_ACTIVE || start()) onLoop();
    };

    size_t printDiag(void) {
        return printDiag(out_);
    };

    size_t printDiag(Print *p) {
        StaticJsonDocument<256> doc;
        doc[FPSTR(str_state)] = getStateStr();
        JsonVariant obj = doc.as<JsonObject>();
        onDiag(obj);
        return serializeJsonPretty(doc, *p);
    };

    virtual void onDiag(const JsonObject &obj) {}

    String getStateStr() {
        return getModuleStateStr(state_);
    }

   protected:
    virtual bool onInit() { return true; };

    virtual void onDeinit(){};

    virtual bool onStart() { return true; }

    virtual void onStop(){};

    virtual void onLoop() = 0;

    virtual Error onExecute(const String &paramStr, const String &valueStr) {
        Error err = Error(ERROR_EXECUTE, FPSTR(str_unsupported));
        return err;
    }

    void setError(const String &str);

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
    ModuleState state_;
    Error error_;
    Print *out_;
    Config *config_;
};

#pragma once

#include "CommonTypes.h"
#include "Core/Queue.h"

using namespace StrUtils;

template <typename T>
class Storable {
   public:
    Storable(size_t capacity = 0) {
        state_ = StoreState::SS_UNSET;
        clearError();
    }

    String getStoreErrorStr(const StoreError error) {
        String str;
        switch (error) {
            case SE_INVALID:
                str = FPSTR(str_invalid);
                break;
            case SE_NOT_EXIST:
                str = FPSTR(str_not_exist);
                break;
            case SE_ERROR_READ:
                str = FPSTR(str_read);
                break;
            case SE_ERROR_WRITE:
                str = FPSTR(str_write);
                break;
            case SE_ERROR_CLOSE:
                str = FPSTR(str_close);
                break;
            case SE_NONE:
                str = FPSTR(str_unset);
                break;
            default:
                str = FPSTR(str_unknown);
                break;
        }
        return str;
    }

    String getStateInfoStr(const StoreState state) {
        PGM_P strP = str_unknown;
        switch (state) {
            case SS_UNSET:
                strP = str_unset;
                break;
            case SS_CLOSED:
                strP = str_close;
                break;
            case SS_READING:
                strP = str_read;
                break;
            case SS_WRITING:
                strP = str_write;
                break;
            case SS_EOF:
                strP = str_eof;
                break;
        };
        return FPSTR(strP);
    }

    const String getStateInfo() { return getStateInfoStr(this->state_); }

    const String getErrorInfo() { return getStoreErrorStr(this->error_); }

    bool read() {
        return set(SS_READING);
    }

    bool write() {
        return set(SS_WRITING);
    }

    StoreState state() { return state_; }

    bool has(StoreError value) { return error_ == value; }

    bool is(StoreState state) { return state_ == state; }

    bool ok() { return !getError(); }

    const StoreError getError() { return error_; };

    void clearError() { error_ = StoreError::SE_NONE; }

    void set(const Queue<T>& data) {
        data_ = data;
    }

    Queue<T>* get() {
        return &data_;
    }

    void setError(StoreError error) { this->error_ = error; }

   protected:
    bool set(const StoreState new_state) {
        if (state_ == SS_UNSET)
            onValidate();

        if (state_ != new_state)
            onStateChange(state_, new_state);

        return !getError();
    }

    virtual bool doExist() = 0;

    virtual bool doClose() = 0;

    virtual bool doOpenRead() = 0;

    virtual bool doRead() = 0;

    virtual bool doOpenWrite() = 0;

    virtual bool doWrite() = 0;

    virtual bool doValidate() = 0;

    void onClose() {
        if (is(SS_CLOSED))
            return;
        if (doClose())
            state_ = SS_CLOSED;
        else
            setError(SE_ERROR_CLOSE);
    }

    void onOpenRead() {
        if (is(SS_READING)) 
            return;
        if (doExist()) {
            if (doOpenRead())
                state_ = SS_READING;
            else
                setError(SE_ERROR_READ);
        } else {
            setError(SE_NOT_EXIST);
        }        
    }

    void onRead() {
        if (is(SS_READING)) {
            doRead();
            set(SS_EOF);
        } else {
            setError(SE_INVALID);
        }
    }

    void onOpenWrite() {
        if (is(SS_WRITING))
            return;

        if (doOpenWrite())
            state_ = SS_WRITING;
        else
            setError(SE_ERROR_WRITE);
    }

    void onWrite() {
        if (!is(SS_WRITING))
            setError(SE_INVALID);
        else if (!doWrite())
            setError(SE_ERROR_WRITE);
    }

    void onValidate() {
        if (doValidate())
            state_ = SS_CLOSED;
        else
            setError(SE_INVALID);
    }

    void onStateChange(StoreState from, const StoreState to) {
        clearError();
        switch (to) {
            case SS_EOF:
            case SS_CLOSED:
                onClose();
                break;
            case SS_READING:
                onClose();
                onOpenRead();
                if (getError())
                    break;
                onRead();
                break;
            case SS_WRITING:
                onClose();
                onOpenWrite();
                if (getError())
                    break;
                onWrite();
                set(SS_EOF);
                break;
            default:
                setError(SE_INVALID);
                break;
        }
    }
   protected:
    Queue<T> data_;
    StoreState state_;
    StoreError error_;
};

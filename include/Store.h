#pragma once

#include "CommonTypes.h"
#include "StringQueue.h"

class Store {
   public:
    Store();
    bool read(StringQueue* data);
    bool write(StringQueue* data);
    void printDiag(Print* p);
    StoreState getState();
    bool getState(StoreState);
    bool getError(StoreError);
    String getStateInfo();
    String getErrorInfo();
    StoreError getError();
    void clearError();

   protected:
    void onStateChange(StoreState state, StoreState to);
    bool setState(const StoreState state);
    virtual bool doExist() = 0;
    virtual bool doClose() = 0;
    virtual bool doOpenRead() = 0;
    virtual bool doRead(StringQueue& data) = 0;
    virtual bool doOpenWrite() = 0;
    virtual bool doWrite(StringQueue& data) = 0;
    virtual bool doValidate() = 0;

   protected:
    static String getErrorInfo(StoreError error) {
        String str;
        switch (error) {
            case SE_INVALID:
                str = getStrP(str_invalid);
                str += getStrP(str_state);
                break;
            case SE_NOT_EXIST:
                str = getStrP(str_not);
                str += getStrP(str_exist);
                break;
            case SE_ERROR_READ:
                str = getStrP(str_read);
                break;
            case SE_ERROR_WRITE:
                str = getStrP(str_write);
                break;
            case SE_ERROR_CLOSE:
                str = getStrP(str_close);
                break;
            case SE_OK: 
                str = getStrP(str_unset);
                break;
            default:
                str = getStrP(str_unknown);
                break;
        }
        str += getStrP(str_error, false);
        return str;
    }

    static String getStateInfo(StoreState state) {
        String str;
        switch (state) {
            case SS_UNSET:
                str = getStrP(str_unset);
                break;
            case SS_CLOSED:
                str = getStrP(str_close);
                break;
            case SS_READING:
                str = getStrP(str_read);
                break;
            case SS_WRITING:
                str = getStrP(str_write);
                break;
            case SS_EOF:
                str = getStrP(str_eof);
                break;
        };
        return str;
    }

   private:
    void onValidate();
    void onClose();
    void onOpenRead();
    void onOpenWrite();
    void onRead();
    void onWrite();
    void setError(StoreError error);

   private:
    StringQueue* data;

   protected:
    Print* dbg = &DEBUG;
    Print* err = &ERROR;
    StoreState state;
    StoreError error;
};

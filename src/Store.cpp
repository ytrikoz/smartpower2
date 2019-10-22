#include "Store.h"

#include "StrUtils.h"
#include "Strings.h"

using namespace StrUtils;

Store::Store() {
    state = StoreState::SS_UNSET;
    clearError();
}

void Store::clearError() { error = StoreError::SE_OK; }

StoreError Store::getError() { return this->error; };

bool Store::read(StringQueue *queue) {
#ifdef DEBUG_FILE_STORAGE
    dbg->print(getIdentStrP(str_store));
    dbg->println(StrUtils::getStrP(str_read));
#endif
    data = queue;
    return setState(SS_READING);
}

bool Store::write(StringQueue *queue) {
#ifdef DEBUG_FILE_STORAGE
    dbg->print(getIdentStrP(str_store));
    dbg->println(StrUtils::getStrP(str_write));
#endif
    data = queue;
    return setState(SS_WRITING);
}

StoreState Store::getState() { return this->state; }

bool Store::getError(StoreError error) { return this->error == error; }

bool Store::getState(StoreState state) { return this->state == state; }

String Store::getStateInfo() { return getStateInfo(getState()); }

String Store::getErrorInfo() { return getErrorInfo(getError()); }

bool Store::setState(const StoreState to) {
    if (getState(SS_UNSET))
        onValidate();

    if (!getState(to)) {
        onStateChange(this->state, to);
        return !getError();
    }
    return !getError();
}

void Store::onClose() {
    if (!getState(SS_CLOSED)) {
        if (doClose())
            state = SS_CLOSED;
        else
            setError(SE_ERROR_CLOSE);
    }
}

void Store::onOpenRead() {
    if (!getState(SS_READING)) {
        if (doExist()) {
            if (doOpenRead())
                state = SS_READING;
            else
                setError(SE_ERROR_READ);
        } else {
            setError(SE_NOT_EXIST);
        }
    }
}

void Store::onRead() {
    if (getState(SS_READING)) {
        doRead(*data);
        setState(SS_EOF);
    } else {
        setError(SE_INVALID);
    }
}

void Store::onOpenWrite() {
    if (!getState(SS_WRITING)) {
        if (doOpenWrite())
            state = SS_WRITING;
        else
            setError(SE_ERROR_WRITE);
    }
}

void Store::onWrite() {
    if (!getState(SS_WRITING)) {
        setError(SE_INVALID);
    } else if (!doWrite(*data)) {
        setError(SE_ERROR_WRITE);
    }
}

void Store::onValidate() {
    if (doValidate()) {
        state = SS_CLOSED;
    } else {
        setError(SE_INVALID);
    }
}

void Store::onStateChange(StoreState state, const StoreState to) {
#ifdef DEBUG_FILE_STORAGE
    dbg->print(getIdentStrP(str_store));
    dbg->print(getStateInfo(state));
    dbg->print(StrUtils::getStrP(str_arrow_dest));
    dbg->println(getStateInfo(to));
#endif
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
        setState(SS_EOF);
        break;
    default:
        setError(SE_INVALID);
        break;
    }
}

void Store::setError(StoreError error) { this->error = error; }

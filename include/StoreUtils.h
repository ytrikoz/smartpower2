#pragma once

#include "Storage.h"

namespace StoreUtils {

inline bool storeString(const char *filename, const String &value) {
    auto store = Storage<File, String>(filename);
    auto container = Container<String>(1);
    store.use(&container);
    store.get()->put(value);
    return store.write();
}

inline bool restoreString(const char *filename, String &value) {
    auto store = FileStorage(filename);
    auto container = Container<String>(1);
    store.use(&container);
    return store.read() && store.get()->get(value);
}

inline bool restoreByte(const char *file, byte &value) {
    String buf;
    if (restoreString(file, buf)) {
        value = buf.toInt();
        return true;
    }
    return false;
}

inline bool restoreInt(const char *file, long &value) {
    String buf;
    if (restoreString(file, buf)) {
        value = buf.toInt();
        return true;
    }
    return false;
}

inline bool restoreDouble(const char *file, double &value) {
    String buf;
    if (restoreString(file, buf)) {
        value = buf.toDouble();
        return true;
    }
    return false;
}

inline bool storeInt(const char *file, long value) {
    return storeString(file, StrUtils::long2str(value));
}

inline bool storeDouble(const char *file, double value) {
    return storeString(file, StrUtils::double2str(value));
}

}  // namespace StoreUtils
#pragma once

#include "Storage.h"

namespace StoreUtils {

inline bool storeString(const char *filename, const String &value) {
    auto file = StringFile(filename);
    auto data = file.get();
    data->push(value);        
    return file.write();
}

inline bool restoreString(const char *filename, String &value) {
    auto file = StringFile(filename);
    auto data = file.get();
    return file.read() && data->pop(value);
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
#pragma once

#include "StrUtils.h"
#include "Core/Storage.h"

#include <Print.h>
#include <time.h>


namespace FSUtils {

inline bool move(const char *src, const char *dst) {
    return LittleFS.exists(src) && LittleFS.rename(src, dst);
}

inline bool exists(const String &name) {
    return LittleFS.exists(name);
}

inline void print(Print *p, const String &name) {
    if (auto f = LittleFS.open(name, "r")) {
        while (f.available()) {
            p->println(f.readStringUntil('\n'));
        }
        f.close();
    }
}

inline const String getFSUsed() {
    FSInfo fsi;
    LittleFS.info(fsi);
    return StrUtils::prettyBytes(fsi.usedBytes);
}

inline const String getFSTotal() {
    FSInfo fsi;
    LittleFS.info(fsi);
    return StrUtils::prettyBytes(fsi.totalBytes);
}

inline bool formatFS() {
    bool res = LittleFS.format();
    return res;
}

inline size_t getNestedLevel(const String &path) {
    size_t res = 0;
    for (size_t i = 0; i < path.length(); ++i)
        if (path.charAt(i) == '/') res++;
    return res;
}

inline const String asDir(const char *path) {
    String res(path);
    if (!res.endsWith("/"))
        res += "/";
    return res;
}

inline size_t getFilesCount(const char *path) {
    String dir = asDir(path);
    size_t level = getNestedLevel(asDir(dir.c_str()));
    Dir d = LittleFS.openDir(path);
    size_t res = 0;
    while (d.next()) {
        if (getNestedLevel(d.fileName()) <= level) res++;
    };
    return res;
}

inline void printFileList(Print *p, const char *path) {
    String dir = asDir(path);
    uint8_t level = getNestedLevel(dir);
    Dir d = LittleFS.openDir(dir);
    while (d.next()) {
        String name = d.fileName();
        if (getNestedLevel(name) <= level) {
            p->print(name);
            p->print(' ');
            p->print(d.fileTime());
            p->print(' ');
            p->println(StrUtils::prettyBytes(d.fileSize()));
        }
    }
}

inline void rmDir(Print *p, const char *path) {
    String dir = asDir(path);
    uint8_t level = getNestedLevel(dir);
    Dir d = LittleFS.openDir(dir);
    while (d.next()) {
        String name = d.fileName();
        if (getNestedLevel(name) <= level) {
            LittleFS.remove(name);
        }
    }
}

inline bool writeString(const char *name, const String &value) {
    bool res = false;
    auto f = LittleFS.open(name, "w");
    if (f) {
        f.println(value);
        f.close();
        res = true;
    }
    return res;
}

inline bool writeTime(const char *file, const time_t value) {
    String buf = String((unsigned long)value);
    return writeString(file, StrUtils::long2str(value));
}

inline bool writeInt(const char *file, long value) {
    return writeString(file, StrUtils::long2str(value));
}

inline bool writeDouble(const char *file, double value) {
    return writeString(file, StrUtils::double2str(value));
}

inline bool readString(const char *name, String &value) {
    bool res = false;
    auto f = LittleFS.open(name, "r");
    if (f) {
        value = f.readStringUntil('\n');
        f.close();
        res = true;
    }
    return res;
}

inline bool readTime(const char *file, time_t &value) {
    String buf;
    if (readString(file, buf)) {
        value = buf.toInt();
        return true;
    }
    return false;
}

inline bool readDouble(const char *name, double &value) {
    String buf;
    if (readString(name, buf)) {
        value = buf.toDouble();
        return true;
    }
    return false;
}

inline bool readInt(const char *name, long &value) {
    String buf;
    if (readString(name, buf)) {
        value = buf.toInt();
        return true;
    }
    return false;
}

}  // namespace FSUtils
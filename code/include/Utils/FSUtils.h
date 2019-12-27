#pragma once

#include "FS.h"
#include "StrUtils.h"
#include <Print.h>
#include <time.h>

namespace FSUtils {

inline bool move(const char *src, const char *dst) {
    return SPIFFS.exists(src) && SPIFFS.rename(src, dst);
}

inline bool exist(const String &name) {
    return SPIFFS.exists(name);
}

inline void print(Print *p, const String &name) {
    if (auto f = SPIFFS.open(name, "r")) {
        while (f.available()) {
            p->println(f.readStringUntil('\n'));
        }
        f.close();
    }
}

const String getFSTotal();

const String getFSUsed();

const String asDir(const String &path);

const String asDir(const char *path);

size_t getNestedLevel(const String &path);

size_t getFilesCount(const char *path);

size_t getFilesCount(String &path);

void printDir(Print *p, const char *path);

void clearDir(Print *p, const char *path);

bool formatFS();

String getFSStats();

bool writeTime(const char *name, const time_t value);

bool writeInt(const char *file, const long value);

bool readInt(const char *name, long &value);

bool readString(const char *name, String &value);

bool readDouble(const char *name, double &value);

bool writeDouble(const char *name, const double value);

bool readTime(const char *name, time_t &value);
}  // namespace FSUtils
#include "Utils/FSUtils.h"

#include "Utils/PrintUtils.h"
#include "Utils/StrUtils.h"

#include "Core/Storage.h"
#include "Strings.h"

using namespace PrintUtils;
using namespace StrUtils;

namespace FSUtils {

const String getFSUsed() {
    FSInfo fsi;
    SPIFFS.info(fsi);    
    return prettyBytes(fsi.usedBytes);
}

const String getFSTotal() {
    FSInfo fsi;
    SPIFFS.info(fsi);
    return prettyBytes(fsi.totalBytes);
}

const String asDir(const String &pathStr) { return asDir(pathStr.c_str()); }

const String asDir(const char *pathStr) {
    String res(pathStr);
    if (!res.startsWith("/"))
        res = "/" + res;
    if (!res.endsWith("/"))
        res += "/";
    return res;
}

size_t getNestedLevel(const String &path) {
    size_t result = 0;
    for (size_t i = 0; i < path.length(); ++i)
        if (path.charAt(i) == '/')
            result++;
    return result;
}

size_t getFilesCount(const String &path) {
    return getFilesCount(path.c_str());
}

size_t getFilesCount(const char *path) {
    size_t max_level = getNestedLevel(asDir(path));
    Dir dir = SPIFFS.openDir(path);
    size_t res = 0;    
    while (dir.next()) {
        auto name = dir.fileName();
        if (getNestedLevel(name) > max_level)
            continue;
        res++;
    };
    return res;
}

void printDir(Print *p, const char *path) {
    String dir_path = asDir(path);
    uint8_t max_level = getNestedLevel(dir_path);
    Dir dir = SPIFFS.openDir(dir_path);
    while (dir.next()) {
        auto name = dir.fileName();
        if (getNestedLevel(name) > max_level)
            continue;
        p->print(dir.fileName());
        p->print('\t');
        p->print(dir.fileTime());
        p->print('\t');
        p->println(prettyBytes(dir.fileSize()).c_str());
        PrintUtils::println(p);
    }
}

void clearDir(Print *p, const char *path) {
    String dir_path = asDir(path);
    uint8_t max_level = getNestedLevel(dir_path);
    Dir dir = SPIFFS.openDir(dir_path);
    while (dir.next()) {
        auto name = dir.fileName();
        if (getNestedLevel(name) > max_level)
            continue;
        SPIFFS.remove(dir.fileName());
    }
}

bool formatFS() {
    bool res = SPIFFS.format();
    return res;
}

bool writeString(const char *name, const String &value) {
    auto file = StringFile(name);
    auto data = file.get();
    data->push(value);        
    return file.write();
}

bool writeTime(const char *file, const time_t value) {
    String buf = String((unsigned long) value);
    return writeString(file, StrUtils::long2str(value));
}

bool writeInt(const char *file, long value) {
    return writeString(file, StrUtils::long2str(value));
}

bool writeDouble(const char *file, double value) {
    return writeString(file, StrUtils::double2str(value));
}

bool readString(const char *name, String &value) {
    auto file = StringFile(name);
    auto data = file.get();
    return file.read() && data->pop(value);
}

bool readTime(const char *file, time_t &value) {
    String buf;
    if (readString(file, buf)) {
        value = buf.toInt();
        return true;
    }
    return false;
}

bool readDouble(const char *name, double &value) {
    String buf;
    if (readString(name, buf)) {
        value = buf.toDouble();
        return true;
    }
    return false;
}

bool readInt(const char *name, long &value) {
    String buf;
    if (readString(name, buf)) {
        value = buf.toInt();
        return true;
    }
    return false;
}

}
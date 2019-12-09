#include "FSUtils.h"

#include "FS.h"
#include "PrintUtils.h"
#include "StrUtils.h"

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
    String name;
    while (dir.next()) {
        name = dir.fileName();
        if (getNestedLevel(name) > max_level)
            continue;
        res++;
    };
    return res;
}

size_t printDir(Print *p, const char *path) {
    String dir_path = asDir(path);
    uint8_t max_level = getNestedLevel(dir_path);
    Dir dir = SPIFFS.openDir(dir_path);
    size_t n = 0;
    while (dir.next()) {
        auto name = dir.fileName();
        if (getNestedLevel(name) > max_level)
            continue;
        n += println(p, name, '\t', prettyBytes(dir.fileSize()));
    }
    return n;
}

size_t clearDir(Print *p, const char *path) {
    String dir_path = asDir(path);
    uint8_t max_level = getNestedLevel(dir_path);
    Dir dir = SPIFFS.openDir(dir_path);
    size_t n = 0;
    while (dir.next()) {
        auto name = dir.fileName();
        if (getNestedLevel(name) > max_level)
            continue;
        n += println(p, name, '\t', prettyBytes(dir.fileSize()));
        SPIFFS.remove(dir.fileName());
    }
    return n;
}
}
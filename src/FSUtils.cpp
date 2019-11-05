#include "FSUtils.h"

#include "FS.h"
#include "PrintUtils.h"
#include "StrUtils.h"

using namespace PrintUtils;
using namespace StrUtils;

String asDir(String &pathStr) { return asDir(pathStr.c_str()); }

String asDir(const char *pathStr) {
    String res(pathStr);
    if (!res.startsWith("/"))
        res = "/" + res;
    if (!res.endsWith("/"))
        res += "/";
    return res;
}

uint8_t getLevel(String &pathStr) {
    uint8_t result = 0;
    for (uint8_t i = 0; i < pathStr.length(); ++i)
        if (pathStr.charAt(i) == '/')
            result++;
    return result;
}

uint8_t getFilesCount(String &pathStr) {
    String path = asDir(pathStr);
    uint8_t level = getLevel(path);
    Dir dir = SPIFFS.openDir(path);
    size_t result = 0;
    while (dir.next()) {
        String name = dir.fileName();
        if (!getLevel(name) > level)
            result++;
    };
    return result;
}

uint8_t getFilesCount(const char *pathStr) {
    String path = asDir(pathStr);
    uint8_t max_level = getLevel(path);
    Dir dir = SPIFFS.openDir(path);
    uint8_t res = 0;
    String name;
    while (dir.next()) {
        name = dir.fileName();
        if (getLevel(name) > max_level)
            continue;
        res++;
    };
    return res;
}

uint8_t fileList(Print *p, const char *pathStr) {
    String path = asDir(pathStr);
    uint8_t max_level = getLevel(path);
    Dir dir = SPIFFS.openDir(path);
    uint8_t res = 0;
    String name;
    while (dir.next()) {
        name = dir.fileName();
        if (getLevel(name) > max_level)
            continue;
        res++;
        print(p, dir.fileName());
        print(p, '\t');
        println(p, fmt_size(dir.fileSize()));
    }
    return res;
}

void clearDir(Print *p, const char *pathStr) {
    String path = asDir(pathStr);
    uint8_t max_level = getLevel(path);
    Dir dir = SPIFFS.openDir(path);
    String name;
    while (dir.next()) {
        name = dir.fileName();
        if (getLevel(name) > max_level)
            continue;
        print(p, dir.fileName());
        print(p, '\t');
        println(p, FPSTR(str_deleted));
        SPIFFS.remove(dir.fileName());
    }
}

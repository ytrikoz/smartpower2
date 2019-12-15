#pragma once

#include "Print.h"
#include "Strings.h"

namespace FSUtils {

const String getFSTotal();

const String getFSUsed();

const String asDir(const String &path);

const String asDir(const char *path);

size_t getNestedLevel(const String &path);

size_t getFilesCount(const char *path);

size_t getFilesCount(String &path);

size_t printDir(Print *p, const char *path);

size_t clearDir(Print *p, const char *path);

bool formatFS();

bool writeTime(const char *name, const time_t value);

bool writeInt(const char *file, const long value);

bool readInt(const char *name, long &value);

bool readString(const char *name, String &value);

bool readDouble(const char *name, double &value);

bool writeDouble(const char *name, const double value);

bool readTime(const char *name, time_t &value);

}
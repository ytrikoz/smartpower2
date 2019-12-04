#pragma once

#include "Print.h"
#include "Strings.h"

const String asDir(const String &path);

const String asDir(const char *path);

size_t getNestedLevel(const String &path);

size_t getFilesCount(const char *path);

size_t getFilesCount(String &path);

size_t printDir(Print *p, const char *path);

size_t rmDir(Print *p, const char *path);
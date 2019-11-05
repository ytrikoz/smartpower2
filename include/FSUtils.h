#pragma once

#include "Print.h"
#include "Strings.h"

String asDir(String &pathStr);

String asDir(const char *pathStr);

uint8_t getLevel(String &pathStr);

size_t getFilesCount(const char *pathStr);

size_t getFilesCount(String &pathStr);

void printFileList(Print *p, const char *pathStr);
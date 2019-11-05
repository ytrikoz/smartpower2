#pragma once

#include "Print.h"
#include "Strings.h"

String asDir(String &pathStr);

String asDir(const char *pathStr);

uint8_t getLevel(String &pathStr);

uint8_t getFilesCount(const char *pathStr);

uint8_t getFilesCount(String &pathStr);

uint8_t fileList(Print *p, const char *pathStr);

void clearDir(Print *p, const char *pathStr);
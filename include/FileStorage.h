#pragma once
#include "FS.h"

#include "consts.h"

class FileStorage {
   public:
    FileStorage(const char* name);
    bool put(const char* str);
    bool get(char* str);

   private:
    char filename[FILENAME_MAX_LENGTH];
};
#pragma once

#include <FS.h>

#include "CommonTypes.h"
#include "StringQueue.h"
#include "Store.h"

class FileStore : public Store {
   public:
    FileStore(const char* name);
    void setName(const char* name);
    char* getName();
    bool available();
    bool openRead();
    bool openWrite();
    void close();
    bool isValid();
    String getStateInfo();

   protected:
    bool doValidate();
    bool doExist();
    bool doClose();
    bool doOpenRead();
    bool doRead(StringQueue& data);
    bool doOpenWrite();
    bool doWrite(StringQueue& data);

   private:
    static bool exists(const char* name);

   private:
    char* name;
    File file;
};

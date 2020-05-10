#pragma once

#include <FS.h>
#include <LittleFS.h>

#include "CommonTypes.h"
#include "Core/Queue.h"
#include "Core/Storable.h"
#include "Utils/FSUtils.h"

template <typename T>
class Storage : public Storable<T> {
   public:
    Storage(const char* name) : Storable<T>() {
        size_t size = strlen(name);
        name_ = new char[size + 1];
        strncpy(name_, name, size);
        name_[size] = '\x00';
    }

    ~Storage() {
        delete name_;
    }
    Queue<T>* data() {
        return &this->data_;
    }

    const char* getName() {
        return name_;
    }

    bool doOpenRead() {
        return file_ = LittleFS.open(name_, "r");
    };

    bool doOpenWrite() { return file_ = LittleFS.open(name_, "w"); };

    bool doRead() {
        while (file_.available()) {
            auto buf = file_.readStringUntil('\n');
            data()->push(buf);
        }
        return data()->available();
    };

    bool doWrite() {
        bool res = false;
        if (data()->available()) {
            while (data()->available()) {
                T buf;
                data()->pop(buf);
                file_.println(buf);
            }
            res = true;
        }
        return res;
    };

    bool doClose() {
        if (file_) {
            file_.flush();
            file_.close();
        }
        return true;
    };

    bool doValidate() {
        return strlen(name_);
    };

    bool doExist() {
        return LittleFS.exists(name_);
    };

   private:
    char *name_;
    File file_;
};

class StringFile : public Storage<String> {
   public:
    StringFile(const String& name) : StringFile(name.c_str()){};
    StringFile(const char* name) : Storage(name){};
};

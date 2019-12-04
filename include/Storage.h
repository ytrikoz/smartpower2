#pragma once

#include <FS.h>

#include "CommonTypes.h"
#include "Container.h"
#include "Storable.h"

template <typename S, typename C>
class Storage : public Storable<C> {
   public:
    Storage(const char* name) : Storable<C>(name) {}

    bool doOpenRead() { return file_ = SPIFFS.open(this->name(), "r"); };

    bool doOpenWrite() { return file_ = SPIFFS.open(this->name(), "w"); };

    bool doRead() {
        this->data_->clear();
        while (file_.available()) {
            auto buf = file_.readStringUntil('\n');
             this->data_->put(buf);
            yield();
        }
        return  this->data_->available();
    };

    bool doWrite() {
        while ( this->data_->available()) {
            String buf;
             this->data_->get(buf);
            file_.println(buf);
            yield();
        }
        return ! this->data_->available();
    };

    bool doClose() {
        if (file_) {
            file_.flush();
            file_.close();
        }
        return true;
    };

    bool doValidate() {
        size_t len = strlen(this->name());
        return len && len < FILENAME_SIZE;
    };

    bool doExist() {
        return exists(this->name());
    };

    bool exists(const char* name) { return SPIFFS.exists(name); };

   private:
    S file_;
};


class FileStorage : public Storage<File, String> {
   public:
    FileStorage(const char* name): Storage(name) {};
    
};

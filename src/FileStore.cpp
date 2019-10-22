#include "FileStore.h"

#include "PrintUtils.h"

FileStore::FileStore(const char *name) { setName(name); }

void FileStore::setName(const char *name) {
    size_t name_len = strlen(name);
    this->name = new char[name_len + 1];
    strcpy(this->name, name);
}

char *FileStore::getName() { return this->name; }

bool FileStore::available() { return file.available(); }

bool FileStore::doOpenRead() { return file = SPIFFS.open(this->name, "r"); };

bool FileStore::doOpenWrite() { return file = SPIFFS.open(this->name, "w"); };

bool FileStore::doRead(StringQueue &data) {
#ifdef DEBUG_FILE_STORAGE
    dbg->print(getIdentStrP(str_file));
    dbg->print(StrUtils::getStrP(str_read));
    dbg->println(getQuotedStr(name));
#endif
    bool result = file.available();
    while (file.available()) {
        String str = file.readStringUntil('\n');
        data.put(str);
    }
    return result;
}

bool FileStore::doWrite(StringQueue &data) {
    String buf = "";
    size_t n = 0;
    while (data.available()) {
        data.get(buf);
        n += file.println(buf);
    }
    return n;
}

bool FileStore::doClose() {
    if (file) {
        file.flush();
        file.close();
    }
    return true;
}

bool FileStore::doValidate() {
    size_t len = strlen(this->name);
    bool result = len > 0 && len < FILENAME_SIZE + 1;
    if (!result) {
        PrintUtils::print_ident(err, FPSTR(str_file));
        PrintUtils::print(err, FPSTR(str_invalid));
        PrintUtils::println(err, StrUtils::getQuotedStr(this->name));
    }
    return result;
}

bool FileStore::doExist() {
    bool res = exists(this->name);
    if (!res) {
        PrintUtils::print_ident(err, FPSTR(str_store));
        PrintUtils::print(err, FPSTR(str_not_exists));
        PrintUtils::println(err, StrUtils::getQuotedStr(this->name));
    }
    return res;
}

bool FileStore::exists(const char *name) { return SPIFFS.exists(name); }
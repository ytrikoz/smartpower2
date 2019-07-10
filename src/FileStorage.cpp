#include "FileStorage.h"

FileStorage::FileStorage(const char* name) { strcpy(filename, name); }

bool FileStorage::store(const char* buf) {
#ifdef DEBUG_FILE_STORAGE
    debug->printf("[fs] %s -> ", filename);
#endif
    bool result = false;
    File f = SPIFFS.open(FILE_TIME_BACKUP, "w");
    if (f) {
        f.println(buf);
        f.close();
#ifdef DEBUG_FILE_STORAGE
        debug->println(buf);
#endif
        result = true;
    }
    return result;
}

bool FileStorage::restore(char* buf) {
#ifdef DEBUG_FILE_STORAGE
    debug->printf("[fs] %s <- ", filename);
#endif
    bool result = false;
    buf[0] = '\x00';
    if (SPIFFS.exists(filename)) {
        File f = SPIFFS.open(filename, "r");
        if (f.available()) {
            strcpy(buf, f.readStringUntil('\n').c_str());
            result = true;
        }
        f.close();
#ifdef DEBUG_FILE_STORAGE
        debug->println(buf);
#endif
    }
    return result;
}

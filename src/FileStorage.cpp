#include "FileStorage.h"

FileStorage::FileStorage(const char* name) {
    memset(&filename[0], 0, sizeof(filename[0]) * FILENAME_MAX_LENGTH + 1);
    strcpy(filename, name); 
}

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
        debug->print(buf);
#endif
    }
    else {
         #ifdef DEBUG_FILE_STORAGE
        debug->printf_P(str_file_not_found);
        #endif
    }
    #ifdef DEBUG_FILE_STORAGE
    debug->println();
    #endif
    return result;
}

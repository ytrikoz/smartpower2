#include "FileStorage.h"

FileStorage::FileStorage(const char* name) {
    memset(&filename[0], 0, sizeof(filename[0]) * FILENAME_MAX_LENGTH + 1);
    strcpy(filename, name); 
}

bool FileStorage::put(const char* buf) {
#ifdef DEBUG_FILE_STORAGE
    DEBUG.printf("[fs] %s -> ", filename);
#endif
    bool result = false;
    File f = SPIFFS.open(FILE_TIME_BACKUP, "w");
    if (f) {
        f.println(buf);
        f.close();
#ifdef DEBUG_FILE_STORAGE
        DEBUG.println(buf);
#endif
        result = true;
    }
   
    return result;
}

bool FileStorage::get(char* buf) {
#ifdef DEBUG_FILE_STORAGE
    DEBUG.printf("[fs] %s <- ", filename);
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
        DEBUG.print(buf);
#endif
    }
    else {
         #ifdef DEBUG_FILE_STORAGE
        DEBUG.printf_P(strf_file_not_found, filename);
        #endif
    }
    #ifdef DEBUG_FILE_STORAGE
    DEBUG.println();
    #endif
    return result;
}

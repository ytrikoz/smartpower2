#pragma once

#include "FileStore.h"
#include "StringQueue.h"

// TODO templates, async calls
namespace StoreUtils {

inline bool restoreString(const char *file, String &value) {
    FileStore *store = new FileStore(file);
    StringQueue *queue = new StringQueue(1);
    bool result = store->read(queue);
    if (result && queue->available()) { queue->get(value); };
    return result;
}

inline bool restoreInt(const char *file, int &value) {
    FileStore *store = new FileStore(file);
    StringQueue *queue = new StringQueue(1);
    bool result = store->read(queue);
    if (result && queue->available()) {
        String buf;
        queue->get(buf);
        value = buf.toInt();
    }
    return result;
}

inline bool restoreDouble(const char *file, double &value) {
    FileStore *store = new FileStore(file);
    StringQueue *queue = new StringQueue(1);
    bool result = store->read(queue);
    if (result && queue->available()) {
        String buf;
        queue->get(buf);
        value = buf.toDouble();
    }
    return result;
}

inline bool storeString(const char *file, String value) {
	FileStore* store = new FileStore(file);
    StringQueue* payload = new StringQueue(1);
    payload->put(value);        
    return store->write(payload);
}

inline bool storeInt(const char *file, int value) {
    FileStore *store = new FileStore(file);
    StringQueue *queue = new StringQueue(1);
    char buf[16];
    sprintf(buf, "%d", value);
    String str(buf);
    queue->put(str);
    return store->write(queue);
}

inline bool storeDouble(const char *file, double value) {
    FileStore *store = new FileStore(file);
    StringQueue *queue = new StringQueue(1);
    char buf[16];
    sprintf(buf, "%.6f", value);
    String str(buf);
    queue->put(str);
    return store->write(queue);
}

}  // namespace StoreUtils
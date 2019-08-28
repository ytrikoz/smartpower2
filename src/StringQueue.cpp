#include "StringQueue.h"

StringQueue::StringQueue(size_t capacity) {
    this->capacity = capacity;
}

void StringQueue::put(String& str) {
#ifdef DEBUG_FILE_STORAGE
    dbg->print(getIdentStrP(str_data));
    dbg->print(getStrP(str_arrow_src));
    dbg->println(str);
#endif
    items.push_back(str);
}

void StringQueue::prepareGet() {
    if (state != QS_GET) {
        std::reverse(items.begin(), items.end());
        state = QS_GET;
    }
}

void StringQueue::preparePut() {
    state = QS_PUT;
}

void  StringQueue::get(String& str) {
    if (available()) {
        prepareGet();
        str = items.back();
        items.pop_back();
    }
#ifdef DEBUG_FILE_STORAGE
    dbg->print(getIdentStrP(str_data));
    dbg->print(getStrP(str_arrow_dest));
    dbg->println(str);
#endif
}

size_t StringQueue::free() { return capacity - items.size(); }

bool StringQueue::available() { return items.size(); }

void StringQueue::clear() { items.clear(); }

size_t StringQueue::printTo(Print& p) const {
    size_t n = 0;
    for (auto it = items.cbegin(); it != items.cend(); it++) n += p.println(*it);
    return n;
}
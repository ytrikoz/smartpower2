#include "str_utils.h"

namespace str_utils {

String formatSize(size_t bytes) {
    if (bytes < 1024)
        return String(bytes) + "B";
    else if (bytes < (1024 * 1024))
        return String(bytes / 1024.0) + "KB";
    else if (bytes < (1024 * 1024 * 1024))
        return String(bytes / 1024.0 / 1024.0) + "MB";
    else
        return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
}

bool setstr(char *desc, const char *src, uint8_t size) {
    if (src == NULL) {
        memset(desc, 0, size);
    } else if (strcmp(src, desc) != 0) {
        uint8_t len = strlen(src);
        if (len != 0) {
            if (len > (size - 1)) len = (size - 1);
            memcpy(desc, src, len);
            desc[len] = '\x00';
        }
        return true;
    }
    return false;
}

String mac2str(uint8 hwaddr[6]) {
    String str = String(hwaddr[0], HEX);
    str += ":";
    str += String(hwaddr[1], HEX);
    str += ":";
    str += String(hwaddr[2], HEX);
    str += ":";
    str += String(hwaddr[3], HEX);
    str += ":";
    str += String(hwaddr[4], HEX);
    str += ":";
    str += String(hwaddr[5], HEX);

    return str;
}

void printWelcomeTo(Print *p) {
    uint8_t width = SCREEN_WIDTH / 2;

    char title[width + 1];
    char decor[width + 1];
    strcpy(title, APPNAME " v" FW_VERSION);
    addPaddingTo(title, CENTER, width, ' ');

    setStrOfChar(decor, '#', width);
    p->println(decor);
    p->println(title);
    p->println(decor);
}

String getStrInMHz(uint32_t hz) {
    char buf[8];
    itoa(hz / ONE_MHz_hz, buf, DEC);
    strcat(buf, "MHz");
    return String(buf);
}

void setStrOfChar(char *str, char chr, uint8_t len) {
    memset(&str[0], chr, sizeof(char) * len);
    str[len - 1] = '\x00';
}

void addPaddingTo(char *str, Align align, uint8_t line_width, const char ch) {
    uint8_t str_len = strlen(str);
    if (str_len > line_width) {
        return;
    }
    char orig_str[str_len + 1];
    uint8_t str_start = 0, padd_start = 0;
    switch (align) {
        // str...
        case LEFT:
            str_start = 0;
            padd_start = str_len;
            break;
        // ...str
        case RIGHT:
            padd_start = 0;
            str_start = line_width - str_len;
            break;
        // ..str..
        case CENTER:
            str_start = (line_width - str_len) / 2;
            padd_start = 0;
            break;
    }

    strcpy(orig_str, str);

    for (uint8_t i = padd_start; i < line_width - 1; i++) {
        // paste str
        if (i >= str_start && (i - str_start) < str_len - 1) {
            str[i] = orig_str[i - str_start];
        }
        // or ...
        else {
            str[i] = ch;
        }
    }
    str[str_len + 1] = '\x00';
}

// http://stackoverflow.com/a/35236734
void stringToBytes(const char *str, char sep, byte *bytes,
                              int len, int base) {
    for (int i = 0; i < len; i++) {
        bytes[i] = strtoul(str, NULL, base);
        str = strchr(str, sep);
        if (str == NULL || *str == '\x00') {
            break;
        }
        str++;
    }
}

}  // namespace str_utils
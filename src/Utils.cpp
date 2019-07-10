#include "str_utils.h"

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

String formatMHz(uint32_t value) {
    char buf[16];
    itoa(value / 1000000, buf, 10);
    strcat(buf, "MHz");

    return String(buf);
}

int quadraticRegression(double volt) {
    double a = 0.0000006562;
    double b = 0.0022084236;
    float c = 4.08;
    double d = b * b - a * (c - volt);
    double root = (-b + sqrt(d)) / a;
    if (root < 0)
        root = 0;
    else if (root > 255)
        root = 255;
    return root;
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

void charsOf(char *str, char chr, uint8_t len) {
    memset(&str[0], chr, sizeof(char) * len);
    str[len - 1] = '\x00';
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

void printWelcome(Print *p) {
    uint8_t width = SCREEN_WIDTH / 2;
    char tmp[width + 1];
    charsOf(tmp, '#', width);

    char title[OUTPUT_MAX_LENGTH];
    strcpy(title, APPNAME " v" FW_VERSION);

    p->println(tmp);
    p->println(marginStr(title, ALIGN_CENTER, width).c_str());
    p->println(tmp);
}

String marginStr(const char *str, TextAligment align, uint8_t width) {
    uint8_t margin;
    if (align == ALIGN_CENTER) {
        margin = (width - strlen(str)) / 2;
    } else if (align == ALIGN_RIGHT) {
        margin = (width - strlen(str));
    } else {
        margin = 0;
    }
    char tmp[width + 1];
    charsOf(tmp, ' ', sizeof(tmp[0]) * (margin));
    strcat(tmp, str);
    return String(tmp);
}

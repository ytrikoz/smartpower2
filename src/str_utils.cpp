#include "str_utils.h"

namespace str_utils {

void printWelcomeTo(Print *p) {
    char title[SCREEN_WIDTH + 1];
    strcpy(title, APPNAME " v" FW_VERSION);
    uint8_t width = SCREEN_WIDTH / 2;
    addPaddingTo(title, str_utils::CENTER, width, ' ');
    char decor[width + 1];
    strOfChar(decor, '#', width);
    p->println(decor);
    p->println(title);
    p->println(decor);
}

String iptoa(IPAddress ip) {
    String res = "";
    for (int i = 0; i < 3; i++) {
        res += String((ip >> (8 * i)) & 0xFF) + ".";
    }
    res += String(((ip >> 8 * 3)) & 0xFF);

    return res;
}

IPAddress atoip(const char *input) {
    uint8_t parts[4] = {0, 0, 0, 0};
    uint8_t part = 0;
    for (uint8_t a = 0; a < strlen(input); a++) {
        uint8_t b = input[a];
        if (b == '.') {
            part++;
            continue;
        }
        parts[part] *= 10;
        parts[part] += b - '0';
    }
    return IPAddress(parts[0], parts[1], parts[2], parts[3]);
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

String formatMac(uint8 hw[6]) {
    char buf[32];
    sprintf_P(buf, strf_mac, hw[0], hw[1], hw[2], hw[3], hw[4], hw[5]);
    return String(buf);
}

String formatInMHz(uint32_t hz) {
    char buf[8];
    itoa(hz / ONE_MHz_hz, buf, DEC);
    strcat(buf, "MHz");
    return String(buf);
}

void strOfChar(char *str, char chr, uint8_t len) {
    memset(&str[0], chr, sizeof(char) * len);
    str[len - 1] = '\x00';
}

void addPaddingTo(char *str, Align align, uint8_t width, const char ch) {
    uint8_t str_len = strlen(str) + 1;
    if (str_len >= width) {
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
            str_start = width - str_len;
            break;
        // ..str..
        case CENTER:
            str_start = (width - str_len) / 2;
            padd_start = 0;
            break;
    }
    strcpy(orig_str, str);
    for (uint8_t i = padd_start; i < width; i++) {
        if (i >= str_start && (i - str_start <= str_len + 3)) {
            str[i] = orig_str[i - str_start];
        } else {
            str[i] = ch;
        }
    }
}

// http://stackoverflow.com/a/35236734
void stringToBytes(const char *str, char sep, byte *bytes, int len, int base) {
    for (int i = 0; i < len; i++) {
        bytes[i] = strtoul(str, NULL, base);
        str = strchr(str, sep);
        if (str == NULL || *str == '\x00') {
            break;
        }
        str++;
    }
}

String formatSocket(IPAddress ip, int port) {
    char str[32];
    strcpy_P(str, ip.toString().c_str());
    uint8_t len = strlen(str);
    str[len] = ':';
    str[len + 1] = '\x00';
    char buf[8];
    strcat(str, itoa(port, buf, DEC));
    return String(str);
}

bool isVaildIp(const char *ipStr) {
    for (size_t i = 0; i < strlen(ipStr); i++) {
        int c = ipStr[i];
        if (c != '.' && (c < '0' || c > '9')) return false;
    }
    return true;
}

}  // namespace str_utils
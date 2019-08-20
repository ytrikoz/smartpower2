#include "StrUtils.h"

#include <Arduino.h>
#include "Strings.h"

namespace StrUtils {

void strfill(char *str, char chr, size_t len) {
    memset(&str[0], chr, sizeof(char) * len);
    str[len - 1] = '\x00';
}

bool strpositiv(String &s) {
    return s.equals("on") || s.equals("+") || s.equals("yes");
}

bool strnegativ(String &s) {
    return s.equals("off") || s.equals("-") || s.equals("no");
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

String iptos(IPAddress ip) {
    return ip.toString();
}

bool setstr(char *dest, const char *src, size_t size) {
    if (src == NULL) {
        memset(dest, 0, size);
    } else if (strcmp(src, dest) != 0) {
        uint8_t len = strlen(src);
        if (len != 0) {
            if (len > size - 1) len = size - 1;
            memcpy(dest, src, len);
            dest[len] = '\x00';
        }
        return true;
    }
    return false;
}

String formatSize(size_t size) {
    if (size < 1024)
        return String(size) + "B";
    else if (size < (1024 * 1024))
        return String(size / 1024.0) + "KB";
    else if (size < (1024 * 1024 * 1024))
        return String(size / 1024.0 / 1024.0) + "MB";
    else
        return String(size / 1024.0 / 1024.0 / 1024.0) + "GB";
}

String formatMac(uint8 hw[6]) {
    char buf[32];
    sprintf_P(buf, strf_mac, hw[0], hw[1], hw[2], hw[3], hw[4], hw[5]);
    return String(buf);
}

String formatInMHz(uint32_t value) {
    char buf[8];
    itoa(value / ONE_MHz_hz, buf, DEC);
    strcat(buf, "MHz");
    return String(buf);
}

void strpadd(char *str, Align align, size_t size, const char ch) {
    uint8_t str_len = strlen(str);
    if (str_len > size) str_len = size;
    char orig_str[str_len + 1];
    strncpy(orig_str, str, str_len);
    strfill(str, ch, size + 1);
    uint8_t str_start = 0;
    switch (align) {
        // [...str]
        case RIGHT:
            str_start = size - str_len;
            break;
        // [..str..]
        case CENTER:
            str_start = floor((float)(size - str_len) / 2);
            break;
        // [str...]
        case LEFT:
        default:
            str_start = 0;
    }
    for (size_t i = 0; i < str_len; ++i) str[i + str_start] = orig_str[i];
}

// http://stackoverflow.com/a/35236734
void stringToBytes(const char *str, char sep, uint8_t *size, int len,
                   int base) {
    for (int i = 0; i < len; i++) {
        size[i] = strtoul(str, NULL, base);
        str = strchr(str, sep);
        if (str == NULL || *str == '\x00') {
            break;
        }
        str++;
    }
}

String getSocketStr(IPAddress ip, int port) {
    char buf[32];
    strcpy(buf, iptos(ip).c_str());
    size_t len = strlen(buf);
    buf[len++] = ':';
    buf[len++] = '\x00';

    char tmp[8];
    strcat(buf, itoa(port, tmp, DEC));
    return String(buf);
}

bool isip(const char *str) {
    for (size_t i = 0; i < strlen(str); i++) {
        int c = str[i];
        if (c != '.' && (c < '0' || c > '9')) return false;
    }
    return true;
}

bool isip(const String &str) { return isip(str.c_str()); }

}  // namespace StrUtils
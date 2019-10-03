#include "StrUtils.h"

namespace StrUtils {

#define ONE_MHz_hz 1000000UL

static const char strf_mac[] PROGMEM = "mac %02x:%02x:%02x:%02x:%02x:%02x";
static const char str_down[] PROGMEM = "down";
static const char str_up[] PROGMEM = "up";

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

String iptos(IPAddress &ip) { return ip.toString(); }

void setnnstr(char *dest, const char *src) {
    if (src == NULL) {
        dest = new char[1];
        dest[0] = '\x00';
    } else {
        size_t len = strlen(src);
        dest = new char[len + 1];
        setstr(dest, src, len);
    }
}

bool setstr(char *dest, const char *src, size_t size) {
    if (src == NULL) {
        memset(dest, 0, size);
    } else if (strcmp(src, dest) != 0) {
        uint8_t len = strlen(src);
        if (len != 0) {
            if (len > size - 1)
                len = size - 1;
            memcpy(dest, src, len);
            dest[len] = '\x00';
        }
        return true;
    }
    return false;
}

String formatSize(size_t size) {
    if (size < 1024)
        return String(size) + "b";
    else if (size < (1024 * 1024))
        return String(size / 1024.0) + "kB";
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
    if (str_len > size)
        str_len = size;
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
    for (size_t i = 0; i < str_len; ++i)
        str[i + str_start] = orig_str[i];
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
        if (c != '.' && (c < '0' || c > '9'))
            return false;
    }
    return true;
}

bool isip(const String &str) { return isip(str.c_str()); }

String getStr(String &str) { return str + " "; }

String getStr(IPAddress &value) {
    String res = value.toString() + " ";
    return res;
}

String getStr(long unsigned int value) {
    String res(value);
    return res;
}

String getStr(const char *str) {
    String res(str);
    return res + " ";
}

String getStr(int num) {
    String res(num, DEC);
    return res + " ";
}

String getBoolStr(bool value, bool space) {
    return String(value ? F("true") : F("false"));
}

String getOnOffStr(bool value, bool space) {
    String res(value ? F("on") : F("off"));
    return res;
}

String getEnabledStr(bool value, bool space) {
    String res(value ? F("enabled") : F("disabled"));
    return res;
}

char *getUpDownStr(char *buf, bool value, bool space) {
    PGM_P strP = value ? str_up : str_down;
    strcpy_P(buf, strP);
    return buf;
}

String getUpDownStr(bool value) {
    PGM_P strP = value ? str_up : str_down;
    return String(FPSTR(strP));
}

String getIdentStrP(PGM_P strP, bool with_space) {
    String str = getStrP(strP, false);
    return getIdentStr(str, with_space);
}

String getQuotedStrP(PGM_P strP, bool with_space, char ch) {
    char buf[64];
    strcpy_P(buf, strP);
    return getQuotedStr(buf, with_space);
}

String getQuotedStr(const char *str, bool with_space, char ch) {
    return getIdentStr(str, with_space, ch, ch);
}

String getQuotedStr(String &str, bool with_space) {
    return getQuotedStr(str.c_str(), with_space, '\'');
}

size_t getStrP(PGM_P strP, char *buf) {
    strcpy_P(buf, strP);
    return strlen(buf);
}

String getStrP(PGM_P strP, bool spaced) {
    char buf[64];
    strcpy_P(buf, strP);
    return String(buf);
}

String getIdentStr(const char *str, bool with_space, char ch) {
    return getIdentStr(str, with_space, ch, ch);
}

String getIdentStr(String &str, bool with_space) {
    return getIdentStr(str.c_str(), with_space);
}

String getIdentStr(const char *str, bool with_space) {
    return getIdentStr(str, with_space, '[', ']');
}

String getIdentStr(const char *str, bool with_space, char left, char right) {
    char buf[32] = {0};
    buf[0] = left;
    strcpy(&buf[1], str);
    size_t x = strlen(buf);
    buf[x] = right;
    if (with_space)
        buf[++x] = ' ';
    return String(buf);
}

template <typename... Args> void sayArgsP(Print *p, Args... args) {
    char buf[128];
    for (size_t n = 0; n < sizeof...(args); ++n) {
        strcpy(buf, (char *)pgm_read_ptr(&args[n]...));
        p->print(buf);
    }
}

} // namespace StrUtils

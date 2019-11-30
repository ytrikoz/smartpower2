#include "PrintUtils.h"

using namespace StrUtils;

namespace PrintUtils {

size_t print_shell_exit(Print *p) { return p->println(FPSTR(msg_shell_exit)); }

size_t print_shell_interrupted(Print *p) {
    return p->println(FPSTR(msg_shell_interrupted));
}
size_t println_done(Print *p) { return println(p, FPSTR(str_done)); }

size_t print_not_found(Print *p, String &str) {
    size_t n = print_quoted(p, str.c_str());
    n += p->print(' ');
    n += p->print(FPSTR(str_not));
    n += p->print(' ');
    return n = p->println(FPSTR(str_found));
}

size_t print_file_not_found(Print *p, String &name) {
    size_t n = print(p, FPSTR(str_file));
    return n += print_not_found(p, name);
}

size_t print_dir_not_found(Print *p, String &name) {
    size_t n = print(p, FPSTR(str_dir));
    return n += print_not_found(p, name);
}

size_t print_ln(Print *p) { return p->println(); }

size_t print_quoted(Print *p, const char *str) {
    size_t n = p->print('\'');
    n += p->print(str);
    n += p->print('\'');
    return n;
}

size_t print_quoted(Print *p, String &str) {
    return print_quoted(p, str.c_str());
}

size_t print_ident(Print *p, const char *str) {
    size_t n = p->print('[');
    n += p->print(str);
    n += p->print(']');
    return n;
}

void print_welcome(Print *p, const char *title, const char *message,
                   const char *footer) {
    size_t width = SCREEN_WIDTH / 2;
    char tmp[width + 1];

    strcpy(tmp, title);
    strpadd(tmp, StrUtils::CENTER, width, '#');
    p->println(tmp);

    strcpy(tmp, message);
    strpadd(tmp, StrUtils::CENTER, width);
    p->println(tmp);

    strcpy(tmp, footer);
    strpadd(tmp, StrUtils::CENTER, width, '#');
    p->println(tmp);
}

void print_delay(Print *p, const char *message, uint8_t wait_s) {
    p->print(message);
    p->print(' ');
    for (uint8_t t = wait_s; t > 0; --t) {
        p->print(t);
        p->print(' ');
        delay(ONE_SECOND_ms);
    }
    p->println();
}

size_t print_unknown_item(Print *p, String &name) {
    size_t n = print(p, FPSTR(str_unknown));
    n += print(p, FPSTR(str_item));
    n += println(p, getQuotedStr(name).c_str());
    return n;
}

size_t print_unknown_param(Print *p, String &name) {
    size_t n = print(p, FPSTR(str_unknown));
    n += print(p, FPSTR(str_param));
    n += println(p, getQuotedStr(name).c_str());
    return n;
}

size_t print_unknown_action(Print *p, String &name) {
    size_t n = print(p, FPSTR(str_unknown));
    n += print(p, FPSTR(str_action));
    n += println(p, getQuotedStr(name).c_str());
    return n;
}

size_t print_name_value(Print *p, String &name, String &value) {
    size_t n = p->print(name.c_str());
    n += p->print(' ');
    n += p->print(": ");
    n += p->print(value.c_str());
    return n;
}

size_t print_param_value(Print *p, const char *param, const char *value) {
    char buf[OUTPUT_MAX_LENGTH];
    sprintf(buf, "%s=\"%s\"", param, value);
    return p->println(buf);
}

size_t print_wifi_ap(Print *p) { return print_ident(p, FPSTR(str_wifi_ap)); }

size_t print_wifi_sta(Print *p) { return print_ident(p, FPSTR(str_wifi_sta)); }

size_t print_wifi_ap_station(Print *p, const uint8_t aid, const uint8_t *mac) {
    size_t n = print_nameP_value(p, str_id, aid);
    n += println_nameP_value(p, str_mac, fmt_mac(mac).c_str());
    return n;
}

} // namespace PrintUtils
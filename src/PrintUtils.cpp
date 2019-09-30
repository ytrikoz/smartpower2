#include "PrintUtils.h"

using namespace StrUtils;

namespace PrintUtils {

size_t print_param_value(Print *p, const char *param, const char *value) {
    char buf[OUTPUT_MAX_LENGTH];
    sprintf(buf, "%s=\"%s\"", param, value);
    return p->println(buf);
}

size_t print_shell_start(Print *p) {
    return p->println(FPSTR(msg_shell_start));
}

size_t print_shell_quit(Print *p) { return p->println(FPSTR(msg_shell_quit)); }

size_t print_shell_interrupted(Print *p) {
    return p->print(FPSTR(msg_session_interrupted));
}

size_t print_file_not_found(Print *p, String &name) {
    size_t n = 0;
    n += p->print(getStrP(str_file));
    n += p->print(' ');
    n += p->print(getQuotedStr(name));
    n += p->print(' ');
    n += p->print(getStrP(str_not));
    n += p->print(' ');
    n += p->print(getStrP(str_found));
    return n;
}

size_t print_ln(Print *p) { return p->println(); }

size_t print_done(Print *p) {
    size_t n = p->print(getStrP(str_done));
    return n;
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

void delay_print(Print *p, const char *message, uint8_t wait_s) {
    p->print(message);
    p->print(' ');
    for (uint8_t t = wait_s; t > 0; --t) {
        p->print(t);
        p->print(' ');
        p->flush();
        delay(ONE_SECOND_ms);
    }
    p->println();
}

size_t print_unknown_item(Print *p, String &name) {
    size_t n = p->print(getStrP(str_unknown));
    n += p->print(' ');
    n += p->print(getStrP(str_item));
    n += p->print(' ');
    n += p->println(getQuotedStr(name));
    return n;
}

size_t print_unknown_param(Print *p, String &name) {
    size_t n = p->print(getStrP(str_unknown));
    n += p->print(' ');
    n += p->print(getStrP(str_param));
    n += p->print(' ');
    n += p->print(getQuotedStr(name));
    return n;
}

size_t print_unknown_action(Print *p, String &name) {
    size_t n = p->print(getStrP(str_unknown));
    n += p->print(' ');
    n += p->print(getStrP(str_action));
    n += p->print(' ');
    n += p->println(getQuotedStr(name));
    return n;
}

size_t print_name_value(Print *p, String &name, String &value) {
    size_t n = p->print(name.c_str());
    n += p->print(' ');
    n += p->print(": ");
    n += p->print(value.c_str());
    return n;
}

} // namespace PrintUtils
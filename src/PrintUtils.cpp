#include "PrintUtils.h"

namespace PrintUtils {

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
    StrUtils::strpadd(tmp, StrUtils::CENTER, width, '#');
    p->println(tmp);

    strcpy(tmp, message);
    StrUtils::strpadd(tmp, StrUtils::CENTER, width);
    p->println(tmp);

    strcpy(tmp, footer);
    StrUtils::strpadd(tmp, StrUtils::CENTER, width, '#');
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

void print_unknown_item(Print *p, String &itemStr) {
    p->print(StrUtils::getStrP(str_unknown));
    p->print(StrUtils::getStrP(str_item));
    p->print(StrUtils::getQuotedStr(itemStr));
}

void print_unknown_param(Print *p, String &paramStr) {
    p->print(StrUtils::getStrP(str_unknown));
    p->print(StrUtils::getStrP(str_param));
    p->print(StrUtils::getQuotedStr(paramStr));
}

void print_unknown_action(Print *p, String &actionStr) {
    p->print(StrUtils::getStrP(str_unknown));
    p->print(StrUtils::getStrP(str_action));
    p->print(StrUtils::getQuotedStr(actionStr));
}

} // namespace PrintUtils
#pragma once

#include "Arduino.h"

#include "Strings.h"

namespace PrintUtils {

template <typename T> size_t print_paramP_value(Print *p, PGM_P strP, T var) {
    size_t n = p->print(StrUtils::getStrP(strP));
    n += p->print('=');
    n += p->print('\"');
    n += p->print(var);
    return n += p->println('\"');
}

template <typename T> size_t println_nameP_value(Print *p, PGM_P strP, T var) {
    size_t n = p->print(FPSTR(strP));
    n += p->print(':');
    n += p->print(' ');
    return n += p->println(var);
}

template <typename T> size_t print_nameP_value(Print *p, PGM_P strP, T var) {
    size_t n = p->print(FPSTR(strP));
    n += p->print(':');
    n += p->print(' ');
    n += p->print(var);
    return n += p->print(' ');
}

template <typename T> size_t print_strP_var(Print *p, PGM_P strP, T var) {
    size_t n = p->print(FPSTR(strP));
    n += p->print(' ');
    n += p->print(var);
    return n += p->print(' ');
}

template <typename T> size_t println_strP_var(Print *p, PGM_P strP, T var) {
    size_t n = p->print(FPSTR(strP));
    n += p->print(' ');
    return n += p->println(var);
}

template <typename T>
size_t printlm_moduleP_nameP_value(Print *p, PGM_P moduleStrP, PGM_P nameStrP,
                                   T value) {
    size_t n = p->print(StrUtils::getIdentStrP(moduleStrP));
    return n += println_nameP_value(p, nameStrP, value);
}

template <typename T> size_t print(Print *p, T v) {
    size_t n = p->print(v);
    return n += p->print(' ');
}

template <typename T> size_t print_ident(Print *p, T v) {
    size_t n = p->print('[');
    n += p->print(v);
    n += p->print(']');
    n += p->print(' ');
    return n;
}

template <typename T, typename... Args>
size_t print(Print *p, T first, Args... args) {
    size_t n = print(p, first);
    n += print(p, args...);
    return n;
}

template <typename T, typename... Args>
size_t println(Print *p, T first, Args... args) {
    size_t n = print(p, first, args...);
    n += p->println();
    return n;
}

size_t print_shell_exit(Print *p);

size_t print_shell_interrupted(Print *p);

void print_welcome(Print *p, const char *title, const char *message,
                   const char *footer);

void print_delay(Print *p, const char *message, uint8_t wait_s);

size_t print_unknown_item(Print *p, String &name);

size_t print_unknown_param(Print *p, String &name);

size_t print_unknown_action(Print *p, String &name);

size_t print_ln(Print *p);

size_t print_done(Print *p);

size_t print_quoted(Print *p, String &str);

size_t print_quoted(Print *p, const char *str);

size_t print_ident(Print *p, const char *str);

size_t print_file_not_found(Print *p, String &name);

size_t print_dir_not_found(Print *p, String &name);

size_t print_not_found(Print *p, String &str);

size_t print_param_value(Print *p, const char *name, const char *value);

size_t print_name_value(Print *p, String &name, String &value);

size_t print_prompt(Print *p);

} // namespace PrintUtils
#include "AppModule.h"

using namespace AppUtils;
using namespace PrintUtils;
using namespace StrUtils;

Named::Named(AppModuleEnum module) {
    this->module = module;
    setstr(name, getModuleName(module).c_str(), 16);
}

size_t Named::log_error(PGM_P errorStrP, const char *paramStr) {
    size_t n = print_ident(err, name);
    return n += println(err, FPSTR(errorStrP));
}

void Named::setOutput(Print *p) { this->out = this->dbg = this->err = p; }

const char *Named::getName() { return name; }

size_t Named::say(char *str) {
    size_t n = print_ident(out, name);
    n += print(out, ' ');
    n += print(out, str);
    return n += print_ln(out);
}

size_t Named::say_strP(PGM_P strP) {
    size_t n = print_ident(out, name);
    char buf[64];
    strcpy_P(buf, strP);
    n += print(out, buf);
    return n += print_ln(out);
}

size_t Named::say_strP(PGM_P strP, int value) {
    String str = String(value, DEC);
    return say_strP(strP, str.c_str());
}

size_t Named::say_strP(PGM_P strP, const char *value) {
    size_t n = print_ident(out, name);
    n += println_nameP_value(out, strP, value);
    return n;
}

size_t AppModule::printDiag(Print *p) { return p->println(F("<empty>")); };

AppModule::AppModule(AppModuleEnum module) : Named(module) {}

void AppModule::init(Config *config) {
    this->config = config;
    setConfig(this->config);
}

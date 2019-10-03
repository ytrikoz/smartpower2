#include "AppModule.h"

#include "AppUtils.h"
#include "PrintUtils.h"

using namespace PrintUtils;
using namespace StrUtils;

Named::Named(AppModuleEnum module) {
    this->module = module;
    name = new char[16];
    name = moduleStr(name, module);
}

void Named::setOutput(Print *p) { this->out = this->dbg = this->err = p; }

const char *Named::getName() { return name; }

size_t Named::print(Print *p, char ch) { return p->print(ch); };

size_t Named::print(Print *p, char *str) { return p->print(str); };

size_t Named::print(Print *p, const char *str, size_t size) {
    char buf[size + 1];
    strncpy(buf, str, size);
    return p->print(buf);
};

size_t Named::print_name(Print *p) {
    size_t n = print(p, '[');
    n += print(p, name);
    n += print(p, ']');
    n += print(p, ' ');
    return n;
}

size_t Named::say(String &str) {
    size_t n = print_name(out);
    n += print(out, str.c_str(), str.length());
    return n += out->println();
};

size_t Named::say(char *str) {
    size_t n = print_name(out);
    n += out->print(str);
    return n += out->println();
}

size_t Named::sayP(PGM_P strP) {
    char buf[64];
    strcpy_P(buf, strP);
    return say(buf);
}

size_t Named::sayP_value(PGM_P strP, const char *value) {
    size_t n = print_name(out);
    char buf[16];
    memset(buf, 0, 16);
    strcpy_P(buf, strP);
    n += print(out, buf);
    n += print(out, ':');
    n += print(out, ' ');
    strcpy(buf, value);
    n += print(out, buf);
    return n += out->println();
}

size_t AppModule::printDiag(Print *p) { return p->println(F("<empty>")); };

AppModule::AppModule(AppModuleEnum module) : Named(module) {}

void AppModule::init(Config *config) {
    this->config = config;
    setConfig(this->config);
}

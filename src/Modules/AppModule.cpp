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

size_t Named::print(Print *p, const char *str) {
    return print(p, (char *)str, strlen(str));
}

size_t Named::print(Print *p, char *str, size_t size) {
    size_t n = 0;
    for (n = 0; n < size; ++n) {
        n += print(p, str[n]);
        if (str[n] == '\x00')
            break;
    }
    return n;
};

size_t Named::say(char *str) {
    size_t n = print_ident(out, name);
    n += print(out, str);
    return n += print_ln(out);
}

size_t Named::say_strP(PGM_P strP) {
    size_t n = print_ident(out, name);
    n += print(out, ' ');
    char buf[64];
    strcpy_P(buf, strP);
    n += print(out, buf);
    return say(buf);
}

size_t Named::say_strP(PGM_P strP, int value) {
    String str = String(value, DEC);
    return say_strP(strP, (char *)str.c_str());
}

size_t Named::say_strP(PGM_P strP, char *value) {
    size_t n = print_ident(out, name);
    n += print(out, ' ');
    n += PrintUtils::print_nameP_value(out, strP, value);
    return n;
}

size_t AppModule::printDiag(Print *p) { return p->println(F("<empty>")); };

AppModule::AppModule(AppModuleEnum module) : Named(module) {}

void AppModule::init(Config *config) {
    this->config = config;
    setConfig(this->config);
}

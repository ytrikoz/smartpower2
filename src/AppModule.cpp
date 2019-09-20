#include "AppModule.h"

static PGM_P module_strP[] PROGMEM = {
    str_btn,  str_clock,  str_led, str_psu, str_psu_log, str_task,  str_shell, str_lcd,
    str_http, str_netsvc, str_ntp, str_telnet, str_shell, str_update};

AppModule::AppModule(AppModuleEnum module) {
    moduleName = getModuleName(module);
}

size_t AppModule::saylnf(const char* fmt, ...) {
    String str = StrUtils::getIdentStr(moduleName);
    size_t n = say(str);
    char buf[128];
    va_list args;
    va_start(args, fmt);
    vsprintf(buf, fmt, args);
    va_end(args);
    n += out->println(buf);
    return n;
}

size_t AppModule::sayln_P(PGM_P pgmStr) {
    String str = StrUtils::getIdentStr(moduleName);
    size_t n = say(str);
    str = StrUtils::getStrP(pgmStr);
    n += out->print(str);
    n += out->println();
    return n;
};

size_t AppModule::say(char* str) { return out->print(str); };

size_t AppModule::say(String& str) { return out->print(str); };

size_t AppModule::saylnf_P(const char* fmt, ...) {
    String str = StrUtils::getIdentStr(moduleName);
    size_t n = say(str);
    return n;
}

String getModuleName(size_t index) {
    return getModuleName(AppModuleEnum(index));
}

String getModuleName(AppModuleEnum module) {
    PGM_P pgmStr = (char*)pgm_read_ptr(&(module_strP[module]));
    return StrUtils::getStrP(pgmStr, false);
}
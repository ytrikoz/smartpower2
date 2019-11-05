#include "CrashReport.h"

#include <FS.h>

#include "BuildConfig.h"

#include "FSUtils.h"
#include "PrintUtils.h"
#include "StrUtils.h"

using namespace PrintUtils;
using namespace StrUtils;

CrashReport::CrashReport() {
    SPIFFS.begin();
    num = getFilesCount(FS_CRASH_ROOT);
}

String CrashReport::getName(uint8_t num, unsigned long uptime) {
    char buf[32];
    sprintf(buf, "%d_%lu", num, uptime);
    return String(buf);
}

bool CrashReport::load(String &name) { return load(name.c_str()); }

bool CrashReport::load(const char *name) {
    if (!SPIFFS.exists(name))
        return false;

    File file = SPIFFS.open(name, "r");
    file.readBytes((char *)&cur, sizeof(cur));
    file.close();

    return true;
}

uint8_t CrashReport::getNum() { return num; }

void CrashReport::print(Print *p) {
    print_nameP_value(p, str_restart_reason, cur.reason);
    println_nameP_value(p, str_exception, cur.exccause);
    p->printf_P(strf_exception, cur.epc1, cur.epc2, cur.epc3, cur.excvaddr,
                cur.depc);
    p->println();
    println(p, FPSTR(str_stack));
    println(p, cur.stack_size);
}
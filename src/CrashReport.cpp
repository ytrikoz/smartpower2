#include "CrashReport.h"

#include <FS.h>

#include "BuildConfig.h"
#include "PrintUtils.h"

using namespace PrintUtils;

bool CrashReport::print(Print *p, const char *name) {
    String crashfile = String(CRASH_ROOT) + name;

    if (!SPIFFS.exists(crashfile))
        return false;

    File file = SPIFFS.open(crashfile, "r");

    print_nameP_value(p, str_crash_report, crashfile.c_str());
    print_nameP_value(p, str_restart_reason,
                      file.readStringUntil('\n').toInt());
    print_nameP_value(p, str_exception_cause,
                      file.readStringUntil('\n').toInt());

    uint32_t epc1, epc2, epc3, excvaddr, depc;
    epc1 = file.readStringUntil('\n').toInt();
    epc2 = file.readStringUntil('\n').toInt();
    epc3 = file.readStringUntil('\n').toInt();
    excvaddr = file.readStringUntil('\n').toInt();
    depc = file.readStringUntil('\n').toInt();
    p->printf(
        "epc1=0x%08x epc2=0x%08x epc3=0x%08x excvaddr=0x%08x depc=0x%08x\n",
        epc1, epc2, epc3, excvaddr, depc);
    uint32_t stackStart, stackEnd;
    stackStart = file.readStringUntil('\n').toInt();
    stackEnd = file.readStringUntil('\n').toInt();
    int16_t stackLength = stackEnd - stackStart;
    uint32_t stackTrace;
    print(p, ">>>stack>>>");
    println(p, stackLength);
    int16_t currentAddress;
    file.close();
    return true;
}
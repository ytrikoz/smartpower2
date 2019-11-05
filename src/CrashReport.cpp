#include "CrashReport.h"

#include "BuildConfig.h"

#include "FSUtils.h"
#include "PrintUtils.h"
#include "StrUtils.h"

using namespace PrintUtils;
using namespace StrUtils;

CrashReport::CrashReport() {}

bool CrashReport::print(Print *p, const char *file) {
    if (!SPIFFS.exists(file))
        return false;

    File f = SPIFFS.open(file, "r");
    if (!f)
        return false;

    this->print(p, f);

    f.close();
    return true;
}

bool CrashReport::print(Print *p, File &f) {
    p->println(FPSTR(str_crash_report));

    AppCrash c;
    f.readBytes((char *)&c, sizeof(c));

    p->printf_P(strf_restart_reason, c.reason, getRestartStr(c.reason).c_str());
    p->printf_P(strf_except_cause, c.exccause,
                getExceptStr(c.exccause).c_str());
    p->printf_P(strf_except, c.epc1, c.epc2, c.epc3, c.excvaddr, c.depc);
    p->printf_P(str_stack);
    int16_t stack_size = c.stack_end - c.stack_start;
    for (int16_t i = 0; i < stack_size; i += 0x10) {
        p->printf("%08x: ", c.stack_start + i);
        for (uint8_t j = 0; j < 4; j++) {
            uint32_t stack_trace = 0;
            if (f.read(reinterpret_cast<uint8_t *>(&stack_trace), 4))
                p->printf("%08x ", stack_trace);
        }
        p->println();
    }
    p->printf_P(str_stack);

    p->println(FPSTR(str_crash_report));

    return true;
}
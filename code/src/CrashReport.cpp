#include "CrashReport.h"

#include "BuildConfig.h"
#include "Utils/FSUtils.h"
#include "Utils/PrintUtils.h"
#include "Utils/StrUtils.h"

using namespace PrintUtils;
using namespace StrUtils;

namespace {
static const char* str_execept_cause[] PROGMEM = {
    str_except_cause_0, str_except_cause_1, str_restart_reason_2,
    str_except_cause_3, str_except_cause_4, str_except_cause_5,
    str_except_cause_6,
    //
    str_except_cause_8, str_except_cause_9,
    //
    str_except_cause_12, str_except_cause_13, str_except_cause_14,
    str_except_cause_15, str_except_cause_16, str_except_cause_17,
    str_except_cause_18,
    //
    str_except_cause_20,
    //
    str_except_cause_24, str_except_cause_25, str_except_cause_26,
    //
    str_except_cause_28, str_except_cause_29, str_except_cause_32_39,
    //
    str_cause_unknown};

void getRestartReason(char* buf, uint32_t reason) {
    PGM_P strP;
    switch (reason) {
        case 0:
            strP = str_restart_reason_0;
            break;
        case 1:
            strP = str_restart_reason_1;
            break;
        case 2:
            strP = str_restart_reason_2;
            break;
        case 3:
            strP = str_restart_reason_3;
            break;
        case 4:
            strP = str_restart_reason_4;
            break;
        case 5:
            strP = str_restart_reason_5;
            break;
        case 6:
            strP = str_restart_reason_6;
            break;
        default:
            strP = str_unknown;
    }
    strcpy_P(buf, strP);
};

void getExceptionCause(char* buf, uint32 except) {
    if ((except == 7) || (except == 10) || (except == 11) ||
        (except == 19) || (except == 21) || (except == 22) ||
        (except == 27) || (except == 30) ||
        (except == 31))
        except = 23;
    else if (except >= 32 && except <= 39)
        except = 22;
    else if (except > 23)
        except = 23;

    strcpy_P(buf, (char*)pgm_read_ptr(&(str_execept_cause[except])));
}

}  // namespace

CrashReport::CrashReport(Stream& r) {
    r.readBytes((char*)&header, sizeof(header));
    dump_size = header.stack_end - header.stack_start;
    if (dump_size > 512)
        dump_size = 512;
    stack_dump = new uint8_t[dump_size];
    r.readBytes(stack_dump, dump_size);
}

CrashReport::~CrashReport() {
    delete[] stack_dump;
}

void CrashReport::printTo(Print& p) {
    char exccause[32];
    getExceptionCause(exccause, header.exccause);
    char reason[32];
    getRestartReason(reason, header.reason);

    p.println(FPSTR(str_crash_report));
    p.printf_P(strf_exception, header.exccause,
               exccause, header.reason,
               reason, header.epc1,
               header.epc2, header.epc3, header.excvaddr, header.depc);

    p.println(FPSTR(str_crash_stack));
    uint32* stack_trace = reinterpret_cast<uint32_t*>(stack_dump);
    for (size_t i = 0; i < dump_size; i += 0x10) {
        p.printf("%08x: ", header.stack_start + i);
        for (size_t j = 0; j < 4; ++j) {
            p.printf("%08x ", *stack_trace);
            stack_trace++;
        }
        p.println();
    }
}
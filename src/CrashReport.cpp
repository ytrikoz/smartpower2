#include "CrashReport.h"

#include "BuildConfig.h"
#include "FSUtils.h"
#include "PrintUtils.h"
#include "StrUtils.h"

using namespace PrintUtils;
using namespace StrUtils;

static const char str_crash_report[] PROGMEM = ">>>crash>>>";
static const char str_crash_stack[] PROGMEM = ">>>stack>>>";
static const char strf_exception[] PROGMEM =
    "Fatal exception:%d (%s)\nFlag:%d (%s)\nepc1:0x%08x epc2:0x%08x epc3:0x%08x excvaddr:0x%08x depc:0x%08x\n";

static const char str_cause_unknown[] PROGMEM = "Unknown";

static const char str_restart_reason_0[] PROGMEM = "Power on";
static const char str_restart_reason_1[] PROGMEM = "Hardware Watchdog";
static const char str_restart_reason_2[] PROGMEM = "Exception";
static const char str_restart_reason_3[] PROGMEM = "Software Watchdog";
static const char str_restart_reason_4[] PROGMEM = "Software/System restart";
static const char str_restart_reason_5[] PROGMEM = "Deep-Sleep Wake";
static const char str_restart_reason_6[] PROGMEM = "External System";

static const char str_except_cause_0[] PROGMEM = "IllegalInstructionCause";
static const char str_except_cause_1[] PROGMEM = "SyscallCause";
static const char str_except_cause_2[] PROGMEM = "InstructionFetchErrorCause";
static const char str_except_cause_3[] PROGMEM = "LoadStoreErrorCause";
static const char str_except_cause_4[] PROGMEM = "Level1InterruptCause";
static const char str_except_cause_5[] PROGMEM = "AllocaCause";
static const char str_except_cause_6[] PROGMEM = "IntegerDivideByZeroCause";
//
static const char str_except_cause_8[] PROGMEM = "PrivilegedCause";
static const char str_except_cause_9[] PROGMEM = "LoadStoreAlignmentCause";
//
static const char str_except_cause_12[] PROGMEM = "InstrPIFDataErrorCause";
static const char str_except_cause_13[] PROGMEM = "LoadStorePIFDataErrorCause";
static const char str_except_cause_14[] PROGMEM = "InstrPIFAddrErrorCause";
static const char str_except_cause_15[] PROGMEM = "LoadStorePIFAddrErrorCause";
static const char str_except_cause_16[] PROGMEM = "InstTLBMissCause";
static const char str_except_cause_17[] PROGMEM = "InstTLBMultiHitCause";
static const char str_except_cause_18[] PROGMEM = "InstFetchPrivilegeCause";
//
static const char str_except_cause_20[] PROGMEM = "InstFetchProhibitedCause";
//
static const char str_except_cause_24[] PROGMEM = "LoadStoreTLBMissCause";
static const char str_except_cause_25[] PROGMEM = "LoadStoreTLBMultiHitCause";
static const char str_except_cause_26[] PROGMEM = "LoadStorePrivilegeCause";
//
static const char str_except_cause_28[] PROGMEM = "LoadProhibitedCause";
static const char str_except_cause_29[] PROGMEM = "StoreProhibitedCause";
//
static const char str_except_cause_32_39[] PROGMEM = "CoprocessornDisabled";

static const char *str_execept_cause[] PROGMEM = {
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

namespace {
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

    strcpy_P(buf, (char *)pgm_read_ptr(&(str_execept_cause[except])));
}

}  // namespace

CrashReport::CrashReport(Stream &r) {
    r.readBytes((char *)&header, sizeof(header));
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
#pragma once

#include <Arduino.h>

struct CrashHeader {
    uint32_t reason;
    uint32_t exccause;
    uint32_t epc1;
    uint32_t epc2;
    uint32_t epc3;
    uint32_t excvaddr;
    uint32_t depc;
    uint32_t stack_start;
    uint32_t stack_end;
};

class CrashReport {
   public:
    CrashReport(Stream& report);
    ~CrashReport();
    void printTo(Print& p);

   private:
    String getName(uint8_t num, unsigned long uptime);

   private:
    CrashHeader header;
    uint8_t* stack_dump;
    uint32_t dump_size;
};

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
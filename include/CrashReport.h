#pragma once

#include <Arduino.h>
#include <FS.h>
#include <Stream.h>
#include <user_interface.h>


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
    CrashReport(Stream &report);
    ~CrashReport();
    void printTo(Print& p);

   private:
    String getName(uint8_t num, unsigned long uptime);

   private:
    CrashHeader header;
    uint8_t* stack_dump;
    uint32_t dump_size;
};


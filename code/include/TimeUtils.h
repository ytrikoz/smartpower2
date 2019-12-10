#pragma once

#include <Arduino.h>
#include <time.h>

#include "Consts.h"

unsigned long millis_since(unsigned long since);

unsigned long millis_passed(unsigned long start, unsigned long finish);

struct EpochTime : public Printable {
   public:
    size_t printTo(Print &p) const { return p.print(epoch_s); }

   public:
    EpochTime() : EpochTime(0) {}

    EpochTime(unsigned long epoch_s) { this->epoch_s = epoch_s; }

    void tick() { this->epoch_s++; };

    unsigned long toEpoch() { return this->epoch_s; }

    String toString() { return String(this->epoch_s); }

    uint8_t asHours(unsigned long s) { return s / ONE_HOUR_s; }

    uint8_t asMinutes(unsigned long s) { return s / ONE_MINUTE_s; }

   private:
    unsigned long epoch_s;
};

namespace TimeUtils {

int timeZoneInSeconds(const byte timeZone);

void format_elapsed_time(char *buf, time_t elapsed);

char* format_elapsed(double elapsed);

char* format_time(time_t time);

}  // namespace TimeUtils


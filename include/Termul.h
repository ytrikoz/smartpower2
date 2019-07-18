#pragma once

#include <Print.h>
#include "SystemClock.h"
#include "consts.h"
#include "types.h"

class Termul : public Print {
   public:
    Termul();    
    void setStream(Stream *s);
    void setEOL(EOLCode code);
    void setControlCodes(bool enabled = true);
    void del();
    void clear_line();
    size_t println(const char *str);
    size_t println(void);
    size_t write(uint8_t c);
    size_t write(const uint8_t *buf, size_t size);
    bool available();
    int read();
   private:
    
    Stream *s;
    bool cc;
    EOLCode eol;
};
   
#include "executors/ClockSetCommand.h"

#include "consts.h"
#include "global.h"
#include "time_utils.h"

namespace executors {

void ClockSetCommand::Execute(Print *p)
{       
    p->print(FPSTR(str_set));
    p->print(FPSTR(str_date));

    char date_str[32];
    strcpy_P(date_str, str_build_date);
  
    tm tm;
    if (decodeDateStr(date_str, tm))
    {
        Date d = Date(tm);
        p->println(d);
    } else 
    {
        p->println("wrong format or date value");
    }

}

size_t ClockSetCommand::printTo(Print& p) const {
    size_t res = p.println("clock set");
    return res;
}

}
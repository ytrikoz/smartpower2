#pragma once

#include <Stream.h>

#include <SimpleCLI.h>

#include "SystemClock.h"
#include "Termul.h"

#include "consts.h"
#include "str_utils.h"
#include "sysinfo.h"
#include "wireless.h"

class Shell {
   public:  
    Shell();
    void setParser(SimpleCLI *cli);   
    void enableWelcome(bool enabled = true);
    void setTermul(Termul *term);
    Termul *getTerm();
    void loop();
   private:
    void prompt();
    void welcome();
    void onOpen();        
    void onClose();
    void onInput(const char*);
    void onTabPress();

    SimpleCLI *parser;
    Termul *t;
    EOLCode eol;
    char prevInput[INPUT_MAX_LENGTH];

    bool welcomeEnabled;
};

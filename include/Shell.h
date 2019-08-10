#pragma once

#include <SimpleCLI.h>
#include <Stream.h>

#include "Consts.h"
#include "StrUtils.h"
#include "SysInfo.h"
#include "SystemClock.h"
#include "Termul.h"
#include "Wireless.h"

class Shell {
   public:
    Shell();
    void setParser(SimpleCLI *cli);
    void enableWelcome(bool enabled = true);
    void setTermul(Termul *term);
    Termul *getTerm();
    void loop();
    bool isActive();

   private:
    void prompt();
    void welcome();
    void onOpen();
    void onClose();
    void onInput(const char *);
    void onTabPress();

    SimpleCLI *parser;
    Termul *t;
    EOLCode eol;
    char prevInput[INPUT_MAX_LENGTH];

    bool welcomeEnabled;
    bool active;
};
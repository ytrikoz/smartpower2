#pragma once

#include <SimpleCLI.h>
#include <Stream.h>

#include "Termul.h"

class Shell {
   public:
    Shell(SimpleCLI *, Termul *);
    void setTerminal(Termul *);
    Termul *getTerm();
    void loop();    
    void setClock(EpochTime& clock);
    void enableWelcome(bool enabled = true);
    void clearHistory();
    void addHistory(const char *);
    bool getHistoryInput(String &);
    void setEditBuffer(String &);
    bool isActive();    
   private:
    void requestHistoryHandler();
    void onSessionOpen();
    void onSessionClose();
    void onSessionData(const char *);
    bool getLastInput(String &);
    size_t print_prompt(Print*);
    size_t print_welcome(Print*);
    SimpleCLI *cli;
    Termul *t;
    std::vector<String> history;
    uint8_t history_size;
    uint8_t history_pos;
    bool welcomeEnabled;
    bool active;
};
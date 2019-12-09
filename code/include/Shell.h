#pragma once

#include <Arduino.h>

#include <SimpleCLI.h>

#include "Termul.h"

class Shell {
  public:
    Shell();
    void setParser(SimpleCLI *);
    void setTerminal(Termul *);
    Termul *getTerminal();
    void loop();
    void enableWelcome(bool = true);
    void clearHistory();
    void addHistory(const char *);
    bool getHistoryInput(String &);
    void setEditBuffer(String &);
    bool isActive();
    bool run(const char *cmdStr);

  private:
    void useHistory();
    void onSessionOpen();
    void onSessionClose();
    void onSessionData(const char *);
    bool getLastInput(String &);
    size_t print_prompt(Print *);
    size_t print_welcome(Print *);

  private:
    SimpleCLI *cli;
    Termul *t;
    std::vector<String> history;
    uint8_t history_size;
    uint8_t history_pos;
    bool welcomeEnabled;
    bool active;
};
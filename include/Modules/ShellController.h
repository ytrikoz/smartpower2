#pragma once

#include <Arduino.h>

#include "AppModule.h"
#include "Cli.h"
#include "Shell.h"

class ShellController : public AppModule {
  public:
    void loop();
    bool begin();

  public:
    bool setLocal();
    bool setRemote(Stream *);

  public:
    ShellController();
    bool isActive();

  private:
    bool active;
    Shell shell;
    Termul term;
};

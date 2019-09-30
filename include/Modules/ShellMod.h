#pragma once

#include <Arduino.h>

#include "AppModule.h"
#include "Cli.h"
#include "Shell.h"

class ShellMod : public AppModule {
  public:
    bool setLocal();
    bool setRemote(Stream *);
    bool isActive();

  public:
    ShellMod();
    bool begin();
    void loop();

  private:
    bool active;
    Shell shell;
    Termul local, remote;
};

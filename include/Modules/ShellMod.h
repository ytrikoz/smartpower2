#pragma once

#include <Arduino.h>

#include "AppModule.h"
#include "Cli.h"
#include "Shell.h"

class ShellMod : public AppModule {
  public:
    void setSerial();
    void setRemote(Stream *);
    bool isActive();
    bool run(const char *strCmd);

  public:
    ShellMod() : AppModule(MOD_SHELL){};

  protected:
    bool onInit();
    bool onStart();
    void onLoop();

  private:
    Shell shell;
    Termul local, remote;
};

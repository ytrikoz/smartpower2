#pragma once

#include <Arduino.h>

#include "AppModule.h"

#include "Global.h"
#include "Shell.h"
#include "Termul.h"

class ShellMod : public AppModule {
  public:
    void setSerial();
    void setRemote(Stream *);
    bool isActive();
    bool run(const char *);
    bool run(const String&);

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

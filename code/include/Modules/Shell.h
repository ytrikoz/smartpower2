#pragma once

#include <Arduino.h>

#include "Module.h"
#include "CommandShell.h"

namespace Modules { 

class Shell : public Module {
  public:
    Shell();
  public:
    void setShell(CommandShell* shell);
    bool isOpen();
  protected:
    bool onInit() override;
    bool onStart() override;
    void onStop()  override;
    void onLoop() override;
  
  private:
    CommandShell* shell_;     
    Terminal* term_;
};

}
#pragma once

#include "Psu.h"
#include "Shell/ParameterlessCommand.h"

namespace CLI {

void init();
void open(Print *);
void close();
bool active();

void onCommandError(cmd_error *e);

void handlePowerCommand(cmd *c);

void onHelpCommand(cmd *c);
void onPrintCommand(cmd *c);
void onFileRemoveCommand(cmd *c);
void onShowCommand(cmd *c);
void onSetCommand(cmd *c);
void onGetCommand(cmd *c);
void onSystemCommand(cmd *c);
void onWifiScanCommand(cmd *c);
void onWifiDiagCommand(cmd *c);
void onClockCommand(cmd *c);

void print_P(const char *);
void onGetConfigParameter(const char *, const char *);
void onConfigParameterChanged(const char *, const char *, const char *);
void onUnknownConfigParameter(const char *);
void commandDone(String &, String &);
void commandResult(const char *, const char *);
void unknownActionParam(const char *, const char *);
void unknownCommandItem(const char* command, const char* item);
}  // namespace CLI
#pragma once

#include <Print.h>
#include <SimpleCLI.h>

namespace Cli {
void init();
void open(Print *);
void close();
bool active();

void onCommandError(cmd_error *e);

void power(cmd *c);
void show(cmd *c);

void onHelpCommand(cmd *c);
void onPrintCommand(cmd *c);
void onFileRemoveCommand(cmd *c);
void onSetCommand(cmd *c);
void onGetCommand(cmd *c);
void onSystemCommand(cmd *c);
void onWifiScanCommand(cmd *c);
void onWifiDiagCommand(cmd *c);
void onClockCommand(cmd *c);

void onGetConfigParameter(const char *, const char *);
void onConfigParameterChanged(const char *, const char *, const char *);
void onCommandDone(String &action, String &param);
void onFileResult(const char *, const char *);

void unknownActionParam(const char *, const char *);
void unknownCommandItem(const char *command, const char *item);
void unknownConfigParam(const char *param);
}  // namespace Cli

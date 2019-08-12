#pragma once

#include <Print.h>
#include <SimpleCLI.h>

namespace Cli {
void init();
void open(Print *);
void close();
bool active();

void onCommandError(cmd_error *e);
void onConfig(cmd *c);
void onPower(cmd *c);
void onShow(cmd *c);
void onHelp(cmd *c);
void onPrint(cmd *c);
void onRemove(cmd *c);
void onSet(cmd *c);
void onGet(cmd *c);
void onSystem(cmd *c);
void onWifiScan(cmd *c);
void onWifiDiag(cmd *c);
void onClock(cmd *c);

void onGetConfigParameter(const char *, const char *);
void onConfigParameterChanged(const char *, const char *, const char *);

void onDone();
void onDone(String &action, String &param);

void onFileResult(const char *, const char *);

void unknownActionParam(const char *, const char *);
void unknownCommandItem(const char *command, const char *item);
void unknownConfigParam(const char *param);

}  // namespace Cli

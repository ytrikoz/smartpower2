#pragma once

#include <Arduino.h>

namespace SysInfo {

const String getUniqueName();
const String getCpuFreq();
const String getFreeSketch();
const String getSketchSize();

const String getFlashMap();
const String getFlashSize();

String getVcc();

String getChipId();

const String getVersionJson();

String getNetworkJson();
String getSystemJson();

const String getHeapStats();
const String getFSStats();

uint8_t getAPClients();
String getAPClientsInfo();

} // namespace SysInfo
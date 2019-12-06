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

String getVersionJson();
String getNetworkJson();
String getSystemJson();

const String getHeapStats();




uint8_t getAPClientsNum();
String getAPClientsInfo();

} // namespace SysInfo
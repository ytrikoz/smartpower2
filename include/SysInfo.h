#pragma once

#include <Arduino.h>

namespace SysInfo {

String getVcc();
String getCpuFreq();
String getChipId();

String getVersionJson();
String getNetworkJson();
String getSystemJson();

String getHeapStats();
String getFreeSketch();
String getFlashMap();
String getFlashSize();

String getFSTotal();
String getFSUsed();
String getFSList();

String getClientsInfo();

}
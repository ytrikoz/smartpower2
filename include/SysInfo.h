#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FS.h>

#include "Strings.h"
#include "StrUtils.h"

void printResetInfo(Print* p);

String getConnectedStationInfo();

String getChipId();

String getVersionInfoJson();
String getNetworkInfoJson();
String getSystemInfoJson();

String getWiFiPhyMode();
String getWiFiRSSI();

String getVcc();
String getHeapFree();
String getHeapFrag();

String getCpuFreq();
String getFreeSketch();
String getFlashMap();
String getFlashSize();

String getSystemUptime();
String getResetReason();

String getFSTotalSpace();
String getFSUsedSpace();
String getFSFileList();

const char* rssi2human(sint8_t rssi);
uint8_t rssi2per(sint8_t rssi);

String getHeapStat();
String getWifiStaStatus();

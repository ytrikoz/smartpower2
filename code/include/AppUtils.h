#pragma once

#include "CommonTypes.h"
#include "Config.h"

namespace AppUtils {


inline String getNetworkConfig(Config *cfg) {
    String res;
    res += cfg->getValueAsByte(WIFI);
    res += ',';
    res += cfg->getValueAsString(SSID);
    res += ',';
    res += cfg->getValueAsString(PASSWORD);
    res += ',';
    res += cfg->getValueAsBool(DHCP);
    res += ',';
    res += cfg->getValueAsString(IPADDR);
    res += ',';
    res += cfg->getValueAsString(NETMASK);
    res += ',';
    res += cfg->getValueAsString(GATEWAY);
    res += ',';
    res += cfg->getValueAsString(DNS);
    return res;
}

}
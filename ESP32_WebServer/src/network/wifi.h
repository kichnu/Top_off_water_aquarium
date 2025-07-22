#ifndef WIFI_H
#define WIFI_H

#include <Arduino.h>

void initializeWiFi();
void updateWiFi();
bool isWiFiConnected();
String getWiFiStatus();

#endif
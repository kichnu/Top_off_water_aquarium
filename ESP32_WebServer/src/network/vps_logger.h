#ifndef VPS_LOGGER_H
#define VPS_LOGGER_H

#include <Arduino.h>

void initializeVPSLogger();
bool logToVPS(const String& event_type, int volume_ml, const String& timestamp);
String calculateHMAC(const String& message);

#endif
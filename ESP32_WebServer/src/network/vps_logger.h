#ifndef VPS_LOGGER_H
#define VPS_LOGGER_H

#include <Arduino.h>

void initVPSLogger();
bool logEventToVPS(const String& eventType, uint16_t volumeML, const String& timestamp);

#endif
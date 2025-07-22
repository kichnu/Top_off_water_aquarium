#ifndef SYSTEM_H
#define SYSTEM_H

#include <Arduino.h>

void initializeSystem();
void updateSystem();
size_t getFreeHeap();
size_t getMinFreeHeap();
float getCPUTemperature();
unsigned long getUptime();
void resetSystem();

#endif
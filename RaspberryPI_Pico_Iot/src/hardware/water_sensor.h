#ifndef HARDWARE_WATER_SENSOR_H
#define HARDWARE_WATER_SENSOR_H

#include <Arduino.h>
#include "../logic/water_system.h"

// ================= FUNKCJE CZUJNIKA WODY =================
void initializeWaterSensor();
WaterLevel readWaterLevel();
bool hasWaterLevelChanged();
WaterLevel getLastWaterLevel();
unsigned long getLastSensorReadTime();

// ================= DIAGNOSTYKA =================
bool isWaterSensorWorking();
String getWaterLevelString(WaterLevel level);
void debugWaterSensor();

// ================= ZMIENNE GLOBALNE =================
extern WaterLevel lastWaterLevel;
extern unsigned long lastSensorRead;

#endif
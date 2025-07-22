#ifndef WATER_SENSOR_H
#define WATER_SENSOR_H

#include <Arduino.h>

struct SensorState {
    bool current_value;
    bool last_stable_value;
    unsigned long last_change_time;
    bool is_stable;
};

void initializeWaterSensors();
void processDualWaterSensors();
String getCombinedWaterStatus();
bool shouldTriggerPump();
bool isWaterNeeded();

#endif
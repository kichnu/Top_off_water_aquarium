#include "water_sensors.h"
#include "../config/hardware_pins.h"
#include "../core/logging.h"

SensorState sensor1;
SensorState sensor2;

void initWaterSensors() {
    pinMode(WATER_SENSOR1_PIN, INPUT_PULLUP);
    pinMode(WATER_SENSOR2_PIN, INPUT_PULLUP);
    
    // Initialize sensor states
    sensor1.currentReading = (digitalRead(WATER_SENSOR1_PIN) == LOW);
    sensor2.currentReading = (digitalRead(WATER_SENSOR2_PIN) == LOW);
    sensor1.stableReading = sensor1.currentReading;
    sensor2.stableReading = sensor2.currentReading;
    sensor1.isStable = true;
    sensor2.isStable = true;
    sensor1.lastChangeTime = 0;
    sensor2.lastChangeTime = 0;
    
    LOG_INFO("Water sensors initialized - Sensor1: %s, Sensor2: %s",
             sensor1.currentReading ? "LOW" : "OK",
             sensor2.currentReading ? "LOW" : "OK");
}

void updateWaterSensors() {
    unsigned long now = millis();
    
    // Read sensors (LOW = water needed due to pull-up)
    bool s1_raw = (digitalRead(WATER_SENSOR1_PIN) == LOW);
    bool s2_raw = (digitalRead(WATER_SENSOR2_PIN) == LOW);
    
    // Debounce sensor 1
    if (s1_raw != sensor1.currentReading) {
        sensor1.currentReading = s1_raw;
        sensor1.lastChangeTime = now;
        sensor1.isStable = false;
    } else if (!sensor1.isStable && (now - sensor1.lastChangeTime > DEBOUNCE_DELAY_MS)) {
        sensor1.stableReading = s1_raw;
        sensor1.isStable = true;
        LOG_INFO("Sensor1 stabilized: %s", s1_raw ? "LOW" : "OK");
    }
    
    // Debounce sensor 2
    if (s2_raw != sensor2.currentReading) {
        sensor2.currentReading = s2_raw;
        sensor2.lastChangeTime = now;
        sensor2.isStable = false;
    } else if (!sensor2.isStable && (now - sensor2.lastChangeTime > DEBOUNCE_DELAY_MS)) {
        sensor2.stableReading = s2_raw;
        sensor2.isStable = true;
        LOG_INFO("Sensor2 stabilized: %s", s2_raw ? "LOW" : "OK");
    }
}

String getWaterStatus() {
    if (!sensor1.isStable || !sensor2.isStable) {
        return "CHECKING";
    }
    
    if (sensor1.stableReading && sensor2.stableReading) {
        return "LOW";
    } else if (!sensor1.stableReading && !sensor2.stableReading) {
        return "OK";
    } else {
        return "PARTIAL"; // One sensor triggered
    }
}

bool isWaterLevelLow() {
    return sensor1.isStable && sensor2.isStable && 
           sensor1.stableReading && sensor2.stableReading;
}

bool shouldActivatePump() {
    return isWaterLevelLow(); // Future: add more logic here
}